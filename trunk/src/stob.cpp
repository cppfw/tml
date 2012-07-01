#include "stob.hpp"

#include <ting/debug.hpp>
#include <ting/Buffer.hpp>



using namespace stob;



void Parser::ParseDataChunk(const ting::Buffer<ting::u8>& chunk, ParseListener& listener){
	for(ting::u8* s = chunk.Begin(); s != chunk.End(); ++s){
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
							throw stob::Exc("Malformed STOB document. Curly braces without preceding string declaraction encountered.");
						}
						++this->nestingLevel;
						this->stringParsed = false;
						listener.OnChildrenParseStarted();
						break;
					case '}':
						
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
			case LINE_COMMENT:
				//TODO:
				break;
			case MULTILINE_COMMENT:
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
