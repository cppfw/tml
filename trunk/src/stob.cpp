#include "stob.hpp"

#include <sstream>

#include <ting/debug.hpp>
#include <ting/Buffer.hpp>



using namespace stob;



void Parser::ParseChar(ting::u8 c, ParseListener& listener){
	switch(this->state){
		case IDLE:
			switch(c){
				case '"':
					this->state = QUOTED_STRING;
					break;
				case '{':
					if(this->nestingLevel == unsigned(-1)){
						throw stob::Exc("Malformed STOB document. Nesting level is too high.");
					}
					if(!this->stringParsed){
						throw stob::Exc("Malformed STOB document. Curly braces without preceding string declaration encountered.");
					}
					++this->nestingLevel;
					this->stringParsed = false;
					listener.OnChildrenParseStarted();
					break;
				case '}':
					if(this->nestingLevel == 0){
						std::stringstream ss;
						ss << "Malformed STOB document. Unexpected '}' at line: ";
						ss << this->curLine;
						throw stob::Exc(ss.str());
					}
					--this->nestingLevel;
					listener.OnChildrenParseFinished();
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
					break;
			}
			break;
		case UNQUOTED_STRING:
			switch(c){
				case ' ':
				case '\r':
				case '\n':
				case '\t':
					//string end
					listener.OnStringParsed(reinterpret_cast<char*>(this->buf->Begin()), this->p - this->buf->Begin());
					this->arrayBuf.Reset();
					this->buf = &this->staticBuf;
					this->state = IDLE;
					return;
//					break;
				default:
					break;
			}
			//fall-through
		case QUOTED_STRING:
			*this->p = c;
			++this->p;
			if(this->p == this->buf->End()){
				ting::Array<ting::u8> a(this->buf->Size() * 2);
				memcpy(a.Begin(), this->buf->Begin(), this->buf->SizeInBytes());
				
				this->p = a.Begin() + this->buf->Size();
				
				this->arrayBuf = a;
				this->buf = &this->arrayBuf; //set reference
			}
			break;
		default:
			ASSERT(false)
			break;
	}
}



void Parser::ParseDataChunk(const ting::Buffer<ting::u8>& chunk, ParseListener& listener){
	for(const ting::u8* s = chunk.Begin(); s != chunk.End(); ++s){
		
		//skip comments if needed
		if(this->commentState != NO_COMMENT){
			switch(this->commentState){
				case LINE_COMMENT:
					for(; s != chunk.End(); ++s){
						if(*s == '\n'){
							this->commentState = NO_COMMENT;
							break;
						}
					}
					break;//~switch
				case MULTILINE_COMMENT:
					if(this->prevChar == '*'){
						if(*s == '/'){
							this->commentState = NO_COMMENT;
							break;//~switch()
						}else{
							this->prevChar = 0;
						}
					}
					for(; s != chunk.End(); ++s){
						if(*s == '*'){
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
		
		
		
		if(this->prevChar != 0){
			if(this->prevChar == '/'){//possible comment sequence
				if(*s == '/'){
					this->commentState = LINE_COMMENT;
				}else if(*s == '*'){
					this->commentState = MULTILINE_COMMENT;
				}else{
					this->ParseChar('/', listener);
					
					//TODO: check if QUOTED_STRING and *s == '"'
					this->ParseChar(*s, listener);
				}
			}else{
				switch(this->state){
					case IDLE:
					case UNQUOTED_STRING:
						ASSERT(false)
						break;
					case QUOTED_STRING:
						ASSERT(this->prevChar == '\\')//escape sequence
						switch(*s){
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
							case 't':
								this->ParseChar('\t', listener);
								break;
							default:
								{
									std::stringstream ss;
									ss << "Malformed document. Unknown escape sequence on line: ";
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
			if(*s == '/'){//possible comment sequence
				this->prevChar = '/';
			}else{
				switch(this->state){
					case QUOTED_STRING:
						switch(*s){
							case '\\': //escape sequence
								this->prevChar = '\\';
								break;
							case '"':
								//string end
								listener.OnStringParsed(reinterpret_cast<char*>(this->buf->Begin()), this->p - this->buf->Begin());
								this->arrayBuf.Reset();
								this->buf = &this->staticBuf;
								this->state = IDLE;
								break;
							case '\r':
							case '\n':
							case '\t':
								//ignore
								break;
						}
						break;
					case UNQUOTED_STRING:
					case IDLE:
						this->ParseChar(*s, listener);
						break;
					default:
						ASSERT(false)
						break;
				}
			}
		}
		
	}//~for(s)
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
	
	if(parser.IsInProgress()){
		throw stob::Exc("Malformed stob document fed. After parsing all the data, the parser remained in the middle of some parsing task.");
	}
}



ting::Ptr<stob::Node> Load(ting::fs::File& fi){
	//TODO:
	
	return ting::Ptr<stob::Node>();
}
