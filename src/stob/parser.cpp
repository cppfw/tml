#include "parser.hpp"

#include <sstream>
#include <cstring>

#include <utki/debug.hpp>

#include "Exc.hpp"



using namespace stob;


namespace{
const size_t bufReserve_d = 0xff;
}



void Parser::appendCharToString(std::uint8_t c){
	this->buf.push_back(c);
}



void Parser::handleLeftCurlyBracket(ParseListener& listener){
	if(this->nestingLevel == unsigned(-1)){
		throw stob::Exc("Malformed STOB document. Nesting level is too high.");
	}
	ASSERT(this->stringParsed)
	
	++this->nestingLevel;
	this->stringParsed = false;
	listener.onChildrenParseStarted();
}



void Parser::handleRightCurlyBracket(ParseListener& listener){
	if(this->nestingLevel == 0){
		std::stringstream ss;
		ss << "Malformed STOB document. Unexpected '}' at line: ";
		ss << this->curLine;
		throw stob::Exc(ss.str());
	}
	--this->nestingLevel;
	this->stringParsed = false;
	listener.onChildrenParseFinished();
}



void Parser::handleStringEnd(ParseListener& listener){
	listener.onStringParsed(utki::wrapBuf(this->buf));
	this->buf.clear();
	this->buf.reserve(bufReserve_d);
	
	this->stringParsed = true;
	this->state = State_e::IDLE;
}



void Parser::parseChar(std::uint8_t c, ParseListener& listener){
	switch(this->state){
		case State_e::IDLE:
			switch(c){
				case '"':
					this->state = State_e::QUOTED_STRING;
					break;
				case '{':
					if(!this->stringParsed){
						//empty string with children
						ASSERT(this->buf.size() == 0)
						this->handleStringEnd(listener);
					}
					this->handleLeftCurlyBracket(listener);
					break;
				case '}':
					this->handleRightCurlyBracket(listener);
					break;
				case '\n':
					++this->curLine;
					break;
				case '\r':
				case ' ':
				case '\t':
					//ignore
					break;
				default:
					this->state = State_e::UNQUOTED_STRING;
					this->appendCharToString(c);
					break;
			}
			break;
		case State_e::UNQUOTED_STRING:
			switch(c){
				case '\n':
					++this->curLine;
				case ' ':
				case '\r':
				case '\t':
				case '{':
				case '}':
				case '"':
					this->handleStringEnd(listener);
					
					switch(c){
						case '{':
							this->handleLeftCurlyBracket(listener);
							break;
						case '}':
							this->handleRightCurlyBracket(listener);
							break;
						case '"':
							//start parsing quoted string right a way
							this->state = State_e::QUOTED_STRING;
							this->stringParsed = false;
							break;
						default:
							break;
					}
					return;
					
				default:
					this->appendCharToString(c);
					break;
			}
			break;
		case State_e::QUOTED_STRING:
			this->appendCharToString(c);
			break;
		default:
			ASSERT(false)
			break;
	}
}



void Parser::preParseChar(std::uint8_t c, ParseListener& listener){
	if(this->prevChar != 0){
		switch(this->state){
			case State_e::IDLE:
			case State_e::UNQUOTED_STRING:
				ASSERT(this->prevChar == '/')//possible comment sequence
				switch(c){
					case '/':
						this->commentState = CommentState_e::LINE_COMMENT;
						break;
					case '*':
						this->commentState = CommentState_e::MULTILINE_COMMENT;
						break;
					default:
						this->parseChar('/', listener);
						this->parseChar(c, listener);
						break;
				}
				break;
			case State_e::QUOTED_STRING:
				ASSERT(this->prevChar == '\\')//escape sequence
				switch(c){
					case '\\'://backslash
						this->parseChar('\\', listener);
						break;
					case '/'://slash
						this->parseChar('/', listener);
						break;
					case '"':
						this->parseChar('"', listener);
						break;
					case 'n':
						this->parseChar('\n', listener);
						break;
					case 'r':
						this->parseChar('\r', listener);
						break;
					case 't':
						this->parseChar('\t', listener);
						break;
					default:
						{
							std::stringstream ss;
							ss << "Malformed document. Unknown escape sequence (\\" << c << ") on line: ";
							ss << this->curLine;
							throw stob::Exc(ss.str());
						}
						break;
				}
				break;
			default:
				ASSERT(false)
				break;
		}
		this->prevChar = 0;
	}else{//~if(this->prevChar != 0)
		switch(this->state){
			case State_e::QUOTED_STRING:
				switch(c){
					case '\\': //escape sequence
						this->prevChar = '\\';
						break;
					case '"':
//							TRACE(<< "qsp = " << std::string(reinterpret_cast<char*>(this->buf->Begin()), 11) << std::endl)
						this->handleStringEnd(listener);
						break;
					case '\n':
						++this->curLine;
					case '\r':
					case '\t':
						//ignore
						break;
					default:
						this->parseChar(c, listener);
						break;
				}
				break;
			case State_e::UNQUOTED_STRING:
			case State_e::IDLE:
				if(c == '/'){//possible comment sequence
					this->prevChar = '/';
				}else{
					this->parseChar(c, listener);
				}
				break;
			default:
				ASSERT(false)
				break;
		}//~switch
	}
}



void Parser::parseDataChunk(const utki::Buf<std::uint8_t> chunk, ParseListener& listener){
	for(auto s = chunk.cbegin(); s != chunk.cend(); ++s){
//		TRACE(<< "Parser::ParseDataChunk(): *s = " << (*s) << std::endl)
		
		//skip comments if needed
		if(this->commentState != CommentState_e::NO_COMMENT){
			switch(this->commentState){
				case CommentState_e::LINE_COMMENT:
					for(;; ++s){
						if(s == chunk.end()){
							return;
						}
						if(*s == '\n'){
							++this->curLine;
							this->commentState = CommentState_e::NO_COMMENT;
							break;//~for
						}
					}
					break;//~switch
				case CommentState_e::MULTILINE_COMMENT:
					if(this->prevChar == '*'){
						this->prevChar = 0;
						if(*s == '/'){
							this->commentState = CommentState_e::NO_COMMENT;
							break;//~switch()
						}
					}
					for(;; ++s){
						if(s == chunk.end()){
							return;
						}
						if(*s == '\n'){
							++this->curLine;
						}else if(*s == '*'){
							this->prevChar = '*';
							break;//~for()
						}
					}
					break;//~switch
				default:
					ASSERT(false)
					break;
			}
			continue;
		}
		
		this->preParseChar(*s, listener);
		
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
	
	std::array<std::uint8_t, 2048> buf; //2kb read buffer.
	
	size_t bytesRead;
	
	do{
		bytesRead = fi.read(utki::wrapBuf(buf));
		
		utki::Buf<std::uint8_t> b(&*buf.begin(), bytesRead);
		parser.parseDataChunk(b, listener);
	}while(bytesRead == buf.size());

	parser.endOfData(listener);
}



void Parser::reset(){
	this->buf.clear();
	this->buf.reserve(bufReserve_d);
	this->curLine = 1;
	this->nestingLevel = 0;
	this->prevChar = 0;
	this->commentState = CommentState_e::NO_COMMENT;
	this->state = State_e::IDLE;
	this->stringParsed = false;
}
