#include "stob.hpp"

#include <sstream>

#include <ting/debug.hpp>
#include <ting/Buffer.hpp>



using namespace stob;



void Parser::AppendCharToString(ting::u8 c){
	*this->p = c;
	++this->p;
	if(this->p == this->buf->End()){
		ting::Array<ting::u8> a(this->buf->Size() * 2);
		memcpy(a.Begin(), this->buf->Begin(), this->buf->SizeInBytes());

		this->p = a.Begin() + this->buf->Size();

		this->arrayBuf = a;
		this->buf = &this->arrayBuf; //set reference
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
	listener.OnChildrenParseFinished();
}



void Parser::HandleStringEnd(ParseListener& listener){
	listener.OnStringParsed(reinterpret_cast<char*>(this->buf->Begin()), this->p - this->buf->Begin());
	this->arrayBuf.Reset();
	this->buf = &this->staticBuf;
	this->p = this->buf->Begin();
	this->stringParsed = true;
	this->state = IDLE;
}



void Parser::ParseChar(ting::u8 c, ParseListener& listener){
	switch(this->state){
		case IDLE:
			switch(c){
				case '"':
					this->state = QUOTED_STRING;
					break;
				case '{':
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
					this->HandleStringEnd(listener);
					
					if(c == '{'){
						this->HandleLeftCurlyBracket(listener);
					}else if(c == '}'){
						this->HandleRightCurlyBracket(listener);
					}
					
					return;
//					break;
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



void Parser::PreParseChar(ting::u8 c, ParseListener& listener){
	if(this->prevChar != 0){
		if(this->prevChar == '/'){//possible comment sequence
			if(c == '/'){
				this->commentState = LINE_COMMENT;
			}else if(c == '*'){
				this->commentState = MULTILINE_COMMENT;
			}else{
				this->ParseChar('/', listener);

				//TODO: check if QUOTED_STRING and c == '"'
				this->ParseChar(c, listener);
			}
		}else{
			switch(this->state){
				case IDLE:
				case UNQUOTED_STRING:
					ASSERT(false)
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
		}
		this->prevChar = 0;
	}else{//~if(this->prevChar == 0)
		if(c == '/'){//possible comment sequence
			this->prevChar = '/';
		}else{
			switch(this->state){
				case QUOTED_STRING:
					switch(c){
						case '\\': //escape sequence
							this->prevChar = '\\';
							break;
						case '"':
//								TRACE(<< "qsp = " << std::string(reinterpret_cast<char*>(this->buf->Begin()), 11) << std::endl)
							this->HandleStringEnd(listener);
							break;
						case '\n':
							++this->curLine;
							//TODO: insert space?
							break;
						case '\r':
							//ignore
							break;
						case '\t':
							//TODO: ignore?
							break;
						default:
							this->ParseChar(c, listener);
							break;
					}
					break;
				case UNQUOTED_STRING:
				case IDLE:
					this->ParseChar(c, listener);
					break;
				default:
					ASSERT(false)
					break;
			}
		}
	}
}



void Parser::ParseDataChunk(const ting::Buffer<ting::u8>& chunk, ParseListener& listener){
	for(const ting::u8* s = chunk.Begin(); s != chunk.End(); ++s){
//		TRACE(<< "Parser::ParseDataChunk(): *s = " << (*s) << std::endl)
		
		//skip comments if needed
		if(this->commentState != NO_COMMENT){
			switch(this->commentState){
				case LINE_COMMENT:
					for(; s != chunk.End(); ++s){
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
					for(; s != chunk.End(); ++s){
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
	
	//TODO: reset parser
}



void stob::Parse(ting::fs::File& fi, ParseListener& listener){
	ting::fs::File::Guard fileGuard(fi, ting::fs::File::READ);
	
	stob::Parser parser;
	
	ting::StaticBuffer<ting::u8, 2048> buf; //2kb read buffer.
	
	size_t bytesRead;
	
	do{
		bytesRead = fi.Read(buf);
		
		parser.ParseDataChunk(
				ting::Buffer<ting::u8>(buf.Begin(), bytesRead),
				listener
			);
	}while(bytesRead == buf.Size());

	parser.EndOfData(listener);
}



ting::Ptr<stob::Node> Load(ting::fs::File& fi){
	//TODO:
	
	return ting::Ptr<stob::Node>();
}
