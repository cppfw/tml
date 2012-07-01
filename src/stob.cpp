#include "stob.hpp"

#include <sstream>

#include <ting/debug.hpp>
#include <ting/Buffer.hpp>



using namespace stob;



void Parser::ParseDataChunk(const ting::Buffer<ting::u8>& chunk, ParseListener& listener){
	for(ting::u8* s = chunk.Begin(); s != chunk.End(); ++s){
		//TODO:
		if(this->commentSeqenceStarted){
			ASSERT(this->commentState == NO_COMMENT)
			switch(*s){
				case '/':
					this->commentState = LINE_COMMENT;
					++s;
					break;
				case '*':
					this->commentState = MULTILINE_COMMENT;
					++s;
					break;
				default:
					//do nothing
					break;
			}
			this->commentSeqenceStarted = false;
			
			//skip comments if needed
			if(this->commentState != NO_COMMENT){
				switch(this->commentState){
					case LINE_COMMENT:
						for(; s != chunk.End(); ++s){
							if(*s == '\n'){
								this->commentState = NO_COMMENT;
								++s;
								break;
							}
						}
						break;//~switch
					case MULTILINE_COMMENT:
						for(; s != chunk.End(); ++s){
							if(*s == '*'){
								++s;
								if(s != chunk.End()){
									if(*s == '/'){
										this->commentState = NO_COMMENT;
										++s;
										break;
									}
								}else{
									break;
								}
							}
						}
						break;//~switch
				}
			}
			continue;
		}
		
		switch(this->state){
			case IDLE:
				switch(*s){
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
						break;
					case '\n':
						++this->curLine;
						break;
					case '\r':
					case ' ':
					case '\t':
						//ignore
						break;
					case '/':
						this->commentSeqenceStarted = true;
						break;
					default:
						this->state = UNQUOTED_STRING;
						break;
				}
				break;
			case QUOTED_STRING:
				//TODO:
				break;
			case UNQUOTED_STRING:
				//TODO:
				break;
			default:
				ASSERT(false)
				break;
		}
	}
}



void Parse(ting::fs::File& fi, ParseListener& listener){
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
}
