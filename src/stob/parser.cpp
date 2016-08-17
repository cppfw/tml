#include "parser.hpp"

#include <sstream>
#include <cstring>

#include <utki/debug.hpp>

#include "Exc.hpp"



using namespace stob;


namespace{
const size_t bufReserve_d = 0xff;
const size_t fileReadChinkSize_c = 0x4ff;
}

void Parser::handleStringParsed(ParseListener& listener){
	listener.onStringParsed(std::string(&*this->buf.begin(), this->buf.size()));
	this->buf.clear();
	this->buf.reserve(bufReserve_d);
}



void Parser::parseDataChunk(const utki::Buf<std::uint8_t> chunk, ParseListener& listener){
	for(auto s = chunk.cbegin(); s != chunk.cend(); ++s){
//		TRACE(<< "Parser::parseDataChunk(): *s = " << (*s) << std::endl)
			auto& c = *s;
			
			switch (this->state) {
				case State_e::IDLE:
					switch (c) {
//						case '/':
//							if(this->buf.size() != 0 && this->buf.back() == '/') {
//								this->buf.pop_back();
//								this->handleStringParsed(listener);
//								this->state = State_e::SINGLE_LINE_COMMENT;
//							}else{
//								this->buf.push_back(c);
//							}
//							break;
//						case '*':
//							if (this->buf.size() != 0 && this->buf.back() == '/') {
//								this->buf.pop_back();
//								this->handleStringParsed(listener);
//								this->state = State_e::MULTILINE_COMMENT;
//							} else {
//								this->buf.push_back(c);
//							}
//							break;
						case ' ':
						case '\n':
						case '\r':
						case '\t':
							break;
						case '{':
							this->handleStringParsed(listener);
							listener.onChildrenParseStarted();
							break;
						case '}':
							this->handleStringParsed(listener);
							listener.onChildrenParseFinished();
							break;
						case '"':
							this->state = State_e.QUOTED_STRING;
							break;
						default:
							this->handleStringParsed(listener);
							this->buf.push_back(c);
							state = State_e::UNQUOTED_STRING;
							break;
					}
				break;

			case UNQUOTED_STRING:
				switch (c) {
				case '/':
					if (prevChar == '/') {
						state = EState.SINGLE_LINE_COMMENT;
					}
					//do not append the character here yet
					break;
				case '*':
					if (prevChar == '/') {
						state = EState.MULTILINE_COMMENT;
					} else {
						curString.append (c);
					}
					break;
                case '"':
                    listener.onStringParsed(curString.toString());
                    curString = new StringBuilder();
                    state = EState.QUOTED_STRING;
                    stringHasJustBeenParsed = true;
                    break;
				case ' ':
				case '\r':
				case '\n':
				case '\t':
					listener.onStringParsed (curString.toString ());
					curString = new StringBuilder ();
					state = EState.IDLE;
					stringHasJustBeenParsed = true;
					break;
				case '{':
					listener.onStringParsed (curString.toString ());
					curString = new StringBuilder ();
					state = EState.IDLE;
					listener.onChildrenParseStarted ();
					stringHasJustBeenParsed = false;
					break;
				case '}':
					listener.onStringParsed (curString.toString ());
					curString = new StringBuilder ();
					state = EState.IDLE;
					stringHasJustBeenParsed = false;
					listener.onChildrenParseFinished ();
					break;
				default:
					if (prevChar == '/') {
						curString.append ('/');
					}
					curString.append (c);
					break;
				}
				break;

			case QUOTED_STRING:
				switch (c) {
				case '"':
					if (prevChar == '\\') {
						curString.append ('\"');
					} else {
						listener.onStringParsed (curString.toString ());
						curString = new StringBuilder ();
						state = EState.IDLE;
						stringHasJustBeenParsed = true;
					}
					break;
				case '\\':
					if (prevChar == '\\') {
						curString.append ('\\');
						c = '\0';//this is to make new prevChar not to be '\' next cycle
					}
					break;
				case '\r':
				case '\n':
				case '\t':
					//ignore
					break;
				default:
					if (prevChar == '\\') {//if escape sequence
						switch (c) {
						case 'r':
							curString.append ('\r');
							break;
						case 'n':
							curString.append ('\n');
							break;
						case 't':
							curString.append ('\t');
							break;
						default:
							//ignore
							break;
						}
					}else{
						curString.append(c);
					}
					break;
				}
				break;
			case SINGLE_LINE_COMMENT:
				if (c == '\n') {
					state = EState.IDLE;
				}
				break;
			case MULTILINE_COMMENT:
				if (c == '/' && prevChar == '*') {
					state = EState.IDLE;
					c = '\0';//this is to make new prevChar not to be '/' next cycle
				}
				break;
			default:
				//Assert(false)
				break;
			}

			prevChar = c;		
	}//~for(s)
}



void Parser::endOfData(ParseListener& listener){
	if(this->state != State_e::IDLE){
		//add new line at the end of data
		this->preParseChar('\n', listener);
	}
	
	if(this->nestingLevel != 0 || this->state != State_e::IDLE){
		throw stob::Exc("Malformed stob document fed. After parsing all the data, the parser remained in the middle of some parsing task.");
	}
	
	this->reset();
}



void stob::parse(const papki::File& fi, ParseListener& listener){
	papki::File::Guard fileGuard(fi);
	
	stob::Parser parser;
	
	std::array<std::uint8_t, fileReadChinkSize_c> buf; //2kb read buffer.
	
	size_t bytesRead;
	
	do{
		bytesRead = fi.read(utki::wrapBuf(buf));
		
		parser.parseDataChunk(utki::Buf<std::uint8_t>(&*buf.begin(), bytesRead), listener);
	}while(bytesRead == buf.size());

	parser.endOfData(listener);
}



void Parser::reset(){
	this->buf.clear();
	this->buf.reserve(bufReserve_d);
	this->curLine = 1;
	this->nestingLevel = 0;
	this->prevChar = 0;
	this->state = State_e::IDLE;
	this->stringParsed = false;
}
