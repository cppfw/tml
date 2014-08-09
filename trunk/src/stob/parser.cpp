#include "parser.hpp"

#include <sstream>

#include <ting/debug.hpp>

#include "Exc.hpp"



using namespace stob;



void Parser::AppendCharToString(std::uint8_t c){
	*this->p = c;
	++this->p;
	if(this->p == this->buf.end()){
		std::vector<std::uint8_t> a(this->buf.size() * 2);
		memcpy(&*a.begin(), this->buf.begin(), this->buf.SizeInBytes());

		this->p = &*a.begin() + this->buf.size();

		this->arrayBuf = a;
		this->buf = this->arrayBuf; //set reference
	}
}



void Parser::HandleLeftCurlyBracket(ParseListener& listener){
	if(this->nestingLevel == unsigned(-1)){
		throw stob::Exc("Malformed STOB document. Nesting level is too high.");
	}
	if(!this->stringParsed){
		throw stob::Exc("Malformed STOB document. Curly braces without preceding string declaration encountered.");
	}
	++this->nestingLevel;
	this->stringParsed = false;
	listener.OnChildrenParseStarted();
}



void Parser::HandleRightCurlyBracket(ParseListener& listener){
	if(this->nestingLevel == 0){
		std::stringstream ss;
		ss << "Malformed STOB document. Unexpected '}' at line: ";
		ss << this->curLine;
		throw stob::Exc(ss.str());
	}
	--this->nestingLevel;
	this->stringParsed = false;
	listener.OnChildrenParseFinished();
}



void Parser::HandleStringEnd(ParseListener& listener){
	size_t size = this->p - this->buf.begin();
	listener.OnStringParsed(ting::ArrayAdaptor<char>(size == 0 ? 0 : reinterpret_cast<char*>(this->buf.begin()), size));
	this->arrayBuf.clear();
	this->buf = this->staticBuf;
	this->p = this->buf.begin();
	this->stringParsed = true;
	this->state = IDLE;
}



void Parser::ParseChar(std::uint8_t c, ParseListener& listener){
	switch(this->state){
		case IDLE:
			switch(c){
				case '"':
					this->state = QUOTED_STRING;
					break;
				case '{':
					if(!this->stringParsed){
						//empty string with children
						ASSERT(this->p == this->buf.begin())
						this->HandleStringEnd(listener);
					}
					this->HandleLeftCurlyBracket(listener);
					break;
				case '}':
					this->HandleRightCurlyBracket(listener);
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
					this->state = UNQUOTED_STRING;
					this->AppendCharToString(c);
					break;
			}
			break;
		case UNQUOTED_STRING:
			switch(c){
				case '\n':
					++this->curLine;
				case ' ':
				case '\r':
				case '\t':
				case '{':
				case '}':
				case '"':
					this->HandleStringEnd(listener);
					
					switch(c){
						case '{':
							this->HandleLeftCurlyBracket(listener);
							break;
						case '}':
							this->HandleRightCurlyBracket(listener);
							break;
						case '"':
							//start parsing quoted string right a way
							this->state = QUOTED_STRING;
							this->stringParsed = false;
							break;
						default:
							break;
					}
					return;
					
				default:
					this->AppendCharToString(c);
					break;
			}
			break;
		case QUOTED_STRING:
			this->AppendCharToString(c);
			break;
		default:
			ASSERT(false)
			break;
	}
}



void Parser::PreParseChar(std::uint8_t c, ParseListener& listener){
	if(this->prevChar != 0){
		switch(this->state){
			case IDLE:
			case UNQUOTED_STRING:
				ASSERT(this->prevChar == '/')//possible comment sequence
				switch(c){
					case '/':
						this->commentState = LINE_COMMENT;
						break;
					case '*':
						this->commentState = MULTILINE_COMMENT;
						break;
					default:
						this->ParseChar('/', listener);
						this->ParseChar(c, listener);
						break;
				}
				break;
			case QUOTED_STRING:
				ASSERT(this->prevChar == '\\')//escape sequence
				switch(c){
					case '\\'://backslash
						this->ParseChar('\\', listener);
						break;
					case '/'://slash
						this->ParseChar('/', listener);
						break;
					case '"':
						this->ParseChar('"', listener);
						break;
					case 'n':
						this->ParseChar('\n', listener);
						break;
					case 'r':
						this->ParseChar('\r', listener);
						break;
					case 't':
						this->ParseChar('\t', listener);
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
			case QUOTED_STRING:
				switch(c){
					case '\\': //escape sequence
						this->prevChar = '\\';
						break;
					case '"':
//							TRACE(<< "qsp = " << std::string(reinterpret_cast<char*>(this->buf->Begin()), 11) << std::endl)
						this->HandleStringEnd(listener);
						break;
					case '\n':
						++this->curLine;
					case '\r':
					case '\t':
						//ignore
						break;
					default:
						this->ParseChar(c, listener);
						break;
				}
				break;
			case UNQUOTED_STRING:
			case IDLE:
				if(c == '/'){//possible comment sequence
					this->prevChar = '/';
				}else{
					this->ParseChar(c, listener);
				}
				break;
			default:
				ASSERT(false)
				break;
		}//~switch
	}
}



void Parser::ParseDataChunk(const ting::ArrayAdaptor<std::uint8_t> chunk, ParseListener& listener){
	for(const std::uint8_t* s = chunk.begin(); s != chunk.end(); ++s){
//		TRACE(<< "Parser::ParseDataChunk(): *s = " << (*s) << std::endl)
		
		//skip comments if needed
		if(this->commentState != NO_COMMENT){
			switch(this->commentState){
				case LINE_COMMENT:
					for(; s != chunk.end(); ++s){
						if(*s == '\n'){
							++this->curLine;
							this->commentState = NO_COMMENT;
							break;
						}
					}
					break;//~switch
				case MULTILINE_COMMENT:
					if(this->prevChar == '*'){
						this->prevChar = 0;
						if(*s == '/'){
							this->commentState = NO_COMMENT;
							break;//~switch()
						}
					}
					for(; s != chunk.end(); ++s){
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
		
		this->PreParseChar(*s, listener);
		
	}//~for(s)
}



void Parser::EndOfData(ParseListener& listener){
	if(this->state != IDLE){
		//add new line at the end of data
		this->PreParseChar('\n', listener);
	}
	
	if(this->nestingLevel != 0 || this->state != IDLE){
		throw stob::Exc("Malformed stob document fed. After parsing all the data, the parser remained in the middle of some parsing task.");
	}
	
	this->Reset();
}



void stob::Parse(ting::fs::File& fi, ParseListener& listener){
	ting::fs::File::Guard fileGuard(fi, ting::fs::File::READ);
	
	stob::Parser parser;
	
	std::array<std::uint8_t, 2048> buf; //2kb read buffer.
	
	size_t bytesRead;
	
	do{
		bytesRead = fi.Read(buf);
		
		ting::ArrayAdaptor<std::uint8_t> b(buf.begin(), bytesRead);
		parser.ParseDataChunk(b, listener);
	}while(bytesRead == buf.size());

	parser.EndOfData(listener);
}
