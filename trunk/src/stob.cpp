#include "stob.hpp"

#include <ting/debug.hpp>
#include <ting/Buffer.hpp>



using namespace stob;



void Parser::ParseDataChunk(const ting::Buffer<ting::u8>& chunk, ParseListener& listener){
	for(ting::u8* s = chunk.Begin(); s != chunk.End(); ++s){
		switch(*s){
			case '"':
				break;
			default:
				//TODO:
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
