/*

	Module Module to support the communities detection from text graph (TEST)

	File: TEST_Module.h
	NOTE: This file has been generated with the macro_module tool, do not modify

*/

#ifndef _H_MODULE_TEST
#define _H_MODULE_TEST


#include "macro_base.h"

#include "BASE_Module.h"
#include "math.h"
#include "GST_Module.h"



// Datas defined in this module

class MRData_TwoInt : public MRData{

public:
	mr_uint num1;
	mr_uint num2;


	MRData_TwoInt() : MRData(){
	}

	~MRData_TwoInt() {
	}

	int parse(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &num1);
		offset += staticVarIntParse( data+offset , &num2);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += staticVarIntSerialize( data+offset , num1);
		offset += staticVarIntSerialize( data+offset , num2);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &num1);
		offset += staticVarIntParse( data+offset , &num2);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return num1%max_num_partitions;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // comparing num1
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing num2
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		return 0; //If everything is equal
	}

	void copyFrom( MRData_TwoInt *other ){
		num1=other->num1;
		num2=other->num2;
	};
	std::string toString(){
		ostringstream o;
		o << num1;

		o<<" ";
		o << num2;

		o<<" ";
		return o.str();
	}

}; //class MRData_TwoInt

class MRData_Huso : public MRData{

public:
	mr_uint id;
	mr_double merid_gra;
	mr_double merid_rad;


	MRData_Huso() : MRData(){
	}

	~MRData_Huso() {
	}

	int parse(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &id);
		merid_gra=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		merid_rad=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += staticVarIntSerialize( data+offset , id);
		*( (mr_double*) (data+offset) )=merid_gra; offset +=  sizeof(mr_double);
		*( (mr_double*) (data+offset) )=merid_rad; offset +=  sizeof(mr_double);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &id);
		offset += sizeof(mr_double);
		offset += sizeof(mr_double);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return id%max_num_partitions;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // comparing id
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing merid_gra
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		{ // comparing merid_rad
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		return 0; //If everything is equal
	}

	void copyFrom( MRData_Huso *other ){
		id=other->id;
		merid_gra=other->merid_gra;
		merid_rad=other->merid_rad;
	};
	std::string toString(){
		ostringstream o;
		o << id;

		o<<" ";
		o << merid_gra;

		o<<" ";
		o << merid_rad;

		o<<" ";
		return o.str();
	}

}; //class MRData_Huso

class MRData_CdrMx : public MRData{

public:
	mr_uint phone;
	mr_uint cell;
	MRData_Date date;
	MRData_Time time;


	MRData_CdrMx() : MRData(){
	}

	~MRData_CdrMx() {
	}

	int parse(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &phone);
		offset += staticVarIntParse( data+offset , &cell);
		offset += date.parse(data+offset);
		offset += time.parse(data+offset);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += staticVarIntSerialize( data+offset , phone);
		offset += staticVarIntSerialize( data+offset , cell);
		offset += date.serialize(data+offset);
		offset += time.serialize(data+offset);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &phone);
		offset += staticVarIntParse( data+offset , &cell);
		offset += date.size(data+offset);
		offset += time.size(data+offset);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return phone%max_num_partitions;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // comparing phone
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing cell
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing date
			MRData_Date _tmp_data;
			int tmp = _tmp_data.compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing time
			MRData_Time _tmp_data;
			int tmp = _tmp_data.compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		return 0; //If everything is equal
	}

	void copyFrom( MRData_CdrMx *other ){
		phone=other->phone;
		cell=other->cell;
		date.copyFrom(&other->date);
		time.copyFrom(&other->time);
	};
	std::string toString(){
		ostringstream o;
		o << phone;

		o<<" ";
		o << cell;

		o<<" ";
		o << date.toString();

		o<<" ";
		o << time.toString();

		o<<" ";
		return o.str();
	}

}; //class MRData_CdrMx

class MRData_CellMx : public MRData{

public:
	mr_uint cell;
	mr_uint bts;
	mr_uint mun;
	mr_uint sta;
	mr_double posx;
	mr_double posy;


	MRData_CellMx() : MRData(){
	}

	~MRData_CellMx() {
	}

	int parse(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &cell);
		offset += staticVarIntParse( data+offset , &bts);
		offset += staticVarIntParse( data+offset , &mun);
		offset += staticVarIntParse( data+offset , &sta);
		posx=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		posy=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += staticVarIntSerialize( data+offset , cell);
		offset += staticVarIntSerialize( data+offset , bts);
		offset += staticVarIntSerialize( data+offset , mun);
		offset += staticVarIntSerialize( data+offset , sta);
		*( (mr_double*) (data+offset) )=posx; offset +=  sizeof(mr_double);
		*( (mr_double*) (data+offset) )=posy; offset +=  sizeof(mr_double);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &cell);
		offset += staticVarIntParse( data+offset , &bts);
		offset += staticVarIntParse( data+offset , &mun);
		offset += staticVarIntParse( data+offset , &sta);
		offset += sizeof(mr_double);
		offset += sizeof(mr_double);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return cell%max_num_partitions;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // comparing cell
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing bts
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing mun
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing sta
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing posx
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		{ // comparing posy
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		return 0; //If everything is equal
	}

	void copyFrom( MRData_CellMx *other ){
		cell=other->cell;
		bts=other->bts;
		mun=other->mun;
		sta=other->sta;
		posx=other->posx;
		posy=other->posy;
	};
	std::string toString(){
		ostringstream o;
		o << cell;

		o<<" ";
		o << bts;

		o<<" ";
		o << mun;

		o<<" ";
		o << sta;

		o<<" ";
		o << posx;

		o<<" ";
		o << posy;

		o<<" ";
		return o.str();
	}

}; //class MRData_CellMx

class MRData_Bts : public MRData{

public:
	mr_uint id;
	mr_float area;
	mr_uint comms;


	MRData_Bts() : MRData(){
	}

	~MRData_Bts() {
	}

	int parse(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &id);
		area=  *( (mr_float*) (data+offset) ); offset +=  sizeof(mr_float);
		offset += staticVarIntParse( data+offset , &comms);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += staticVarIntSerialize( data+offset , id);
		*( (mr_float*) (data+offset) )=area; offset +=  sizeof(mr_float);
		offset += staticVarIntSerialize( data+offset , comms);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &id);
		offset += sizeof(mr_float);
		offset += staticVarIntParse( data+offset , &comms);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return id%max_num_partitions;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // comparing id
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing area
			if  ( *((mr_float*)(data1 + (*offset1))) < *((mr_float*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_float*)(data1 + (*offset1))) > *((mr_float*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_float);
			*offset2 +=sizeof(mr_float);
		}
		{ // comparing comms
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		return 0; //If everything is equal
	}

	void copyFrom( MRData_Bts *other ){
		id=other->id;
		area=other->area;
		comms=other->comms;
	};
	std::string toString(){
		ostringstream o;
		o << id;

		o<<" ";
		o << area;

		o<<" ";
		o << comms;

		o<<" ";
		return o.str();
	}

}; //class MRData_Bts

class MRData_Poi : public MRData{

public:
	mr_uint id;
	mr_uint node;
	mr_uint bts;
	mr_uint labelnode;
	mr_uint labelgroupnode;
	mr_uint8 confidentnode;
	mr_double distancenode;
	mr_uint labelbts;
	mr_uint labelgroupbts;
	mr_uint8 confidentbts;
	mr_double distancebts;
	mr_uint labelnodebts;
	mr_uint labelgroupnodebts;
	mr_uint8 confidentnodebts;
	mr_double distancenodebts;
	mr_int8 inoutWeek;
	mr_int8 inoutWend;


	MRData_Poi() : MRData(){
	}

	~MRData_Poi() {
	}

	int parse(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &id);
		offset += staticVarIntParse( data+offset , &node);
		offset += staticVarIntParse( data+offset , &bts);
		offset += staticVarIntParse( data+offset , &labelnode);
		offset += staticVarIntParse( data+offset , &labelgroupnode);
		confidentnode=  *( (mr_uint8*) (data+offset) ); offset +=  sizeof(mr_uint8);
		distancenode=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		offset += staticVarIntParse( data+offset , &labelbts);
		offset += staticVarIntParse( data+offset , &labelgroupbts);
		confidentbts=  *( (mr_uint8*) (data+offset) ); offset +=  sizeof(mr_uint8);
		distancebts=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		offset += staticVarIntParse( data+offset , &labelnodebts);
		offset += staticVarIntParse( data+offset , &labelgroupnodebts);
		confidentnodebts=  *( (mr_uint8*) (data+offset) ); offset +=  sizeof(mr_uint8);
		distancenodebts=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		inoutWeek=  *( (mr_int8*) (data+offset) ); offset +=  sizeof(mr_int8);
		inoutWend=  *( (mr_int8*) (data+offset) ); offset +=  sizeof(mr_int8);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += staticVarIntSerialize( data+offset , id);
		offset += staticVarIntSerialize( data+offset , node);
		offset += staticVarIntSerialize( data+offset , bts);
		offset += staticVarIntSerialize( data+offset , labelnode);
		offset += staticVarIntSerialize( data+offset , labelgroupnode);
		*( (mr_uint8*) (data+offset) )=confidentnode; offset +=  sizeof(mr_uint8);
		*( (mr_double*) (data+offset) )=distancenode; offset +=  sizeof(mr_double);
		offset += staticVarIntSerialize( data+offset , labelbts);
		offset += staticVarIntSerialize( data+offset , labelgroupbts);
		*( (mr_uint8*) (data+offset) )=confidentbts; offset +=  sizeof(mr_uint8);
		*( (mr_double*) (data+offset) )=distancebts; offset +=  sizeof(mr_double);
		offset += staticVarIntSerialize( data+offset , labelnodebts);
		offset += staticVarIntSerialize( data+offset , labelgroupnodebts);
		*( (mr_uint8*) (data+offset) )=confidentnodebts; offset +=  sizeof(mr_uint8);
		*( (mr_double*) (data+offset) )=distancenodebts; offset +=  sizeof(mr_double);
		*( (mr_int8*) (data+offset) )=inoutWeek; offset +=  sizeof(mr_int8);
		*( (mr_int8*) (data+offset) )=inoutWend; offset +=  sizeof(mr_int8);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &id);
		offset += staticVarIntParse( data+offset , &node);
		offset += staticVarIntParse( data+offset , &bts);
		offset += staticVarIntParse( data+offset , &labelnode);
		offset += staticVarIntParse( data+offset , &labelgroupnode);
		offset += sizeof(mr_uint8);
		offset += sizeof(mr_double);
		offset += staticVarIntParse( data+offset , &labelbts);
		offset += staticVarIntParse( data+offset , &labelgroupbts);
		offset += sizeof(mr_uint8);
		offset += sizeof(mr_double);
		offset += staticVarIntParse( data+offset , &labelnodebts);
		offset += staticVarIntParse( data+offset , &labelgroupnodebts);
		offset += sizeof(mr_uint8);
		offset += sizeof(mr_double);
		offset += sizeof(mr_int8);
		offset += sizeof(mr_int8);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return id%max_num_partitions;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // comparing id
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing node
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing bts
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing labelnode
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing labelgroupnode
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing confidentnode
			if  ( *((mr_uint8*)(data1 + (*offset1))) < *((mr_uint8*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_uint8*)(data1 + (*offset1))) > *((mr_uint8*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_uint8);
			*offset2 +=sizeof(mr_uint8);
		}
		{ // comparing distancenode
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		{ // comparing labelbts
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing labelgroupbts
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing confidentbts
			if  ( *((mr_uint8*)(data1 + (*offset1))) < *((mr_uint8*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_uint8*)(data1 + (*offset1))) > *((mr_uint8*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_uint8);
			*offset2 +=sizeof(mr_uint8);
		}
		{ // comparing distancebts
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		{ // comparing labelnodebts
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing labelgroupnodebts
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing confidentnodebts
			if  ( *((mr_uint8*)(data1 + (*offset1))) < *((mr_uint8*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_uint8*)(data1 + (*offset1))) > *((mr_uint8*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_uint8);
			*offset2 +=sizeof(mr_uint8);
		}
		{ // comparing distancenodebts
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		{ // comparing inoutWeek
			if  ( *((mr_int8*)(data1 + (*offset1))) < *((mr_int8*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_int8*)(data1 + (*offset1))) > *((mr_int8*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_int8);
			*offset2 +=sizeof(mr_int8);
		}
		{ // comparing inoutWend
			if  ( *((mr_int8*)(data1 + (*offset1))) < *((mr_int8*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_int8*)(data1 + (*offset1))) > *((mr_int8*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_int8);
			*offset2 +=sizeof(mr_int8);
		}
		return 0; //If everything is equal
	}

	void copyFrom( MRData_Poi *other ){
		id=other->id;
		node=other->node;
		bts=other->bts;
		labelnode=other->labelnode;
		labelgroupnode=other->labelgroupnode;
		confidentnode=other->confidentnode;
		distancenode=other->distancenode;
		labelbts=other->labelbts;
		labelgroupbts=other->labelgroupbts;
		confidentbts=other->confidentbts;
		distancebts=other->distancebts;
		labelnodebts=other->labelnodebts;
		labelgroupnodebts=other->labelgroupnodebts;
		confidentnodebts=other->confidentnodebts;
		distancenodebts=other->distancenodebts;
		inoutWeek=other->inoutWeek;
		inoutWend=other->inoutWend;
	};
	std::string toString(){
		ostringstream o;
		o << id;

		o<<" ";
		o << node;

		o<<" ";
		o << bts;

		o<<" ";
		o << labelnode;

		o<<" ";
		o << labelgroupnode;

		o<<" ";
		o << (int)confidentnode;

		o<<" ";
		o << distancenode;

		o<<" ";
		o << labelbts;

		o<<" ";
		o << labelgroupbts;

		o<<" ";
		o << (int)confidentbts;

		o<<" ";
		o << distancebts;

		o<<" ";
		o << labelnodebts;

		o<<" ";
		o << labelgroupnodebts;

		o<<" ";
		o << (int)confidentnodebts;

		o<<" ";
		o << distancenodebts;

		o<<" ";
		o << (int)inoutWeek;

		o<<" ";
		o << (int)inoutWend;

		o<<" ";
		return o.str();
	}

}; //class MRData_Poi

class MRData_Node_Bts : public MRData{

public:
	mr_uint phone;
	mr_uint bts;
	mr_uint8 wday;
	mr_uint range;


	MRData_Node_Bts() : MRData(){
	}

	~MRData_Node_Bts() {
	}

	int parse(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &phone);
		offset += staticVarIntParse( data+offset , &bts);
		wday=  *( (mr_uint8*) (data+offset) ); offset +=  sizeof(mr_uint8);
		offset += staticVarIntParse( data+offset , &range);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += staticVarIntSerialize( data+offset , phone);
		offset += staticVarIntSerialize( data+offset , bts);
		*( (mr_uint8*) (data+offset) )=wday; offset +=  sizeof(mr_uint8);
		offset += staticVarIntSerialize( data+offset , range);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &phone);
		offset += staticVarIntParse( data+offset , &bts);
		offset += sizeof(mr_uint8);
		offset += staticVarIntParse( data+offset , &range);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return phone%max_num_partitions;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // comparing phone
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing bts
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing wday
			if  ( *((mr_uint8*)(data1 + (*offset1))) < *((mr_uint8*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_uint8*)(data1 + (*offset1))) > *((mr_uint8*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_uint8);
			*offset2 +=sizeof(mr_uint8);
		}
		{ // comparing range
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		return 0; //If everything is equal
	}

	void copyFrom( MRData_Node_Bts *other ){
		phone=other->phone;
		bts=other->bts;
		wday=other->wday;
		range=other->range;
	};
	std::string toString(){
		ostringstream o;
		o << phone;

		o<<" ";
		o << bts;

		o<<" ";
		o << (int)wday;

		o<<" ";
		o << range;

		o<<" ";
		return o.str();
	}

}; //class MRData_Node_Bts

class MRData_Bts_Counter : public MRData{

public:
	mr_uint bts;
	mr_uint8 wday;
	mr_uint range;
	mr_uint count;


	MRData_Bts_Counter() : MRData(){
	}

	~MRData_Bts_Counter() {
	}

	int parse(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &bts);
		wday=  *( (mr_uint8*) (data+offset) ); offset +=  sizeof(mr_uint8);
		offset += staticVarIntParse( data+offset , &range);
		offset += staticVarIntParse( data+offset , &count);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += staticVarIntSerialize( data+offset , bts);
		*( (mr_uint8*) (data+offset) )=wday; offset +=  sizeof(mr_uint8);
		offset += staticVarIntSerialize( data+offset , range);
		offset += staticVarIntSerialize( data+offset , count);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &bts);
		offset += sizeof(mr_uint8);
		offset += staticVarIntParse( data+offset , &range);
		offset += staticVarIntParse( data+offset , &count);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return bts%max_num_partitions;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // comparing bts
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing wday
			if  ( *((mr_uint8*)(data1 + (*offset1))) < *((mr_uint8*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_uint8*)(data1 + (*offset1))) > *((mr_uint8*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_uint8);
			*offset2 +=sizeof(mr_uint8);
		}
		{ // comparing range
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing count
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		return 0; //If everything is equal
	}

	void copyFrom( MRData_Bts_Counter *other ){
		bts=other->bts;
		wday=other->wday;
		range=other->range;
		count=other->count;
	};
	std::string toString(){
		ostringstream o;
		o << bts;

		o<<" ";
		o << (int)wday;

		o<<" ";
		o << range;

		o<<" ";
		o << count;

		o<<" ";
		return o.str();
	}

}; //class MRData_Bts_Counter

class MRData_NodeMx_Counter : public MRData{

public:
	MRData_Bts_Counter *bts;
	int bts_length;
	int bts_max_length;


	MRData_NodeMx_Counter() : MRData(){
		bts_length=0;
		bts_max_length=0;
		bts = NULL;
	}

	~MRData_NodeMx_Counter() {
		if( bts )
			delete[] bts ;
	}

	int parse(char *data){
		int offset=0;
		{ //Parsing vector bts
			size_t _length;
			offset += staticVarIntParse( data+offset , &_length );
		 	btsSetLength( _length );
			for (int i = 0 ; i < (int)bts_length ; i++){
				offset += bts[i].parse(data+offset);
			}
		}
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		{ //Serialization vector bts
			offset += staticVarIntSerialize( data+offset , bts_length );
			for (int i = 0 ; i < (int)bts_length ; i++){
				offset += bts[i].serialize(data+offset);
			}
		}
		return offset;
	}

	int size(char *data){
		int offset=0;
		{ //Getting size of vector bts
			size_t _length;
			offset += staticVarIntParse( data+offset , &_length );
			MRData_Bts_Counter _tmp;
			for (int i = 0 ; i < (int)_length ; i++){
				offset += _tmp.size(data+offset);
			}
		}
		return offset;
	}

	int getPartition(int max_num_partitions){
		if( bts_length > 0 ){
		return bts[0].getPartition(max_num_partitions);
		} else return 0;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // Comparing vector bts
			size_t _length1,_length2;
			*offset1 += staticVarIntParse( data1+(*offset1) , &_length1 );
			*offset2 += staticVarIntParse( data2+(*offset2) , &_length2 );
			if( _length1 < _length2 ) return -1;
			if( _length1 > _length2 ) return 1;
			for (int i = 0 ; i < (int)_length1 ; i++){
				{ // comparing bts[i]
					MRData_Bts_Counter _tmp_data;
					int tmp = _tmp_data.compare(data1,data2,offset1 , offset2);
					if( tmp != 0) return tmp;
				}
			}
		}
		return 0; //If everything is equal
	}

	void btsSetLength(int _length){
		if( _length > bts_max_length){ 
			MRData_Bts_Counter *_previous = bts;
			int previous_length = bts_length;
			if(bts_max_length == 0) bts_max_length = _length;
			while(bts_max_length < _length) bts_max_length *= 2;
			bts = new MRData_Bts_Counter[bts_max_length ];
			if( _previous ){
				for (int i = 0 ; i < previous_length ; i++)
					bts[i].copyFrom( &_previous[i] );
				delete[] _previous;
			}
		}
		bts_length=_length;
	}

	MRData_Bts_Counter* btsAdd(){
		btsSetLength( bts_length + 1 );
		return &bts[bts_length-1];
	}

	void copyFrom( MRData_NodeMx_Counter *other ){
			{ // CopyFrom field bts
				btsSetLength( other->bts_length);
				for (int i = 0 ; i < bts_length ; i++){
					bts[i].copyFrom(&other->bts[i]);
				}
			}
	};
	std::string toString(){
		ostringstream o;
		{// toString of vector bts
			for(int i = 0 ; i < bts_length ; i++){
				o << bts[i].toString();
				 o << " ";
			}
		}
		o<<" ";
		return o.str();
	}

}; //class MRData_NodeMx_Counter

class MRData_Node_Bts_Day : public MRData{

public:
	mr_uint node;
	mr_uint bts;
	mr_uint8 workday;
	mr_uint count;


	MRData_Node_Bts_Day() : MRData(){
	}

	~MRData_Node_Bts_Day() {
	}

	int parse(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &node);
		offset += staticVarIntParse( data+offset , &bts);
		workday=  *( (mr_uint8*) (data+offset) ); offset +=  sizeof(mr_uint8);
		offset += staticVarIntParse( data+offset , &count);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += staticVarIntSerialize( data+offset , node);
		offset += staticVarIntSerialize( data+offset , bts);
		*( (mr_uint8*) (data+offset) )=workday; offset +=  sizeof(mr_uint8);
		offset += staticVarIntSerialize( data+offset , count);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &node);
		offset += staticVarIntParse( data+offset , &bts);
		offset += sizeof(mr_uint8);
		offset += staticVarIntParse( data+offset , &count);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return node%max_num_partitions;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // comparing node
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing bts
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing workday
			if  ( *((mr_uint8*)(data1 + (*offset1))) < *((mr_uint8*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_uint8*)(data1 + (*offset1))) > *((mr_uint8*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_uint8);
			*offset2 +=sizeof(mr_uint8);
		}
		{ // comparing count
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		return 0; //If everything is equal
	}

	void copyFrom( MRData_Node_Bts_Day *other ){
		node=other->node;
		bts=other->bts;
		workday=other->workday;
		count=other->count;
	};
	std::string toString(){
		ostringstream o;
		o << node;

		o<<" ";
		o << bts;

		o<<" ";
		o << (int)workday;

		o<<" ";
		o << count;

		o<<" ";
		return o.str();
	}

}; //class MRData_Node_Bts_Day

class MRData_Double : public MRData{

public:
	mr_double value;


	MRData_Double() : MRData(){
	}

	~MRData_Double() {
	}

	int parse(char *data){
		int offset=0;
		value=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		*( (mr_double*) (data+offset) )=value; offset +=  sizeof(mr_double);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += sizeof(mr_double);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return abs( (int) (value*100))%max_num_partitions;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // comparing value
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		return 0; //If everything is equal
	}

	void copyFrom( MRData_Double *other ){
		value=other->value;
	};
	std::string toString(){
		ostringstream o;
		o << value;

		o<<" ";
		return o.str();
	}

}; //class MRData_Double

class MRData_DateVector : public MRData{

public:
	mr_uint8 day;
	mr_uint8 month;
	mr_uint8 hour;


	MRData_DateVector() : MRData(){
	}

	~MRData_DateVector() {
	}

	int parse(char *data){
		int offset=0;
		day=  *( (mr_uint8*) (data+offset) ); offset +=  sizeof(mr_uint8);
		month=  *( (mr_uint8*) (data+offset) ); offset +=  sizeof(mr_uint8);
		hour=  *( (mr_uint8*) (data+offset) ); offset +=  sizeof(mr_uint8);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		*( (mr_uint8*) (data+offset) )=day; offset +=  sizeof(mr_uint8);
		*( (mr_uint8*) (data+offset) )=month; offset +=  sizeof(mr_uint8);
		*( (mr_uint8*) (data+offset) )=hour; offset +=  sizeof(mr_uint8);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += sizeof(mr_uint8);
		offset += sizeof(mr_uint8);
		offset += sizeof(mr_uint8);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return day%max_num_partitions;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // comparing day
			if  ( *((mr_uint8*)(data1 + (*offset1))) < *((mr_uint8*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_uint8*)(data1 + (*offset1))) > *((mr_uint8*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_uint8);
			*offset2 +=sizeof(mr_uint8);
		}
		{ // comparing month
			if  ( *((mr_uint8*)(data1 + (*offset1))) < *((mr_uint8*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_uint8*)(data1 + (*offset1))) > *((mr_uint8*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_uint8);
			*offset2 +=sizeof(mr_uint8);
		}
		{ // comparing hour
			if  ( *((mr_uint8*)(data1 + (*offset1))) < *((mr_uint8*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_uint8*)(data1 + (*offset1))) > *((mr_uint8*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_uint8);
			*offset2 +=sizeof(mr_uint8);
		}
		return 0; //If everything is equal
	}

	void copyFrom( MRData_DateVector *other ){
		day=other->day;
		month=other->month;
		hour=other->hour;
	};
	std::string toString(){
		ostringstream o;
		o << (int)day;

		o<<" ";
		o << (int)month;

		o<<" ";
		o << (int)hour;

		o<<" ";
		return o.str();
	}

}; //class MRData_DateVector

class MRData_DailyVector : public MRData{

public:
	MRData_TwoInt *hours;
	int hours_length;
	int hours_max_length;


	MRData_DailyVector() : MRData(){
		hours_length=0;
		hours_max_length=0;
		hours = NULL;
	}

	~MRData_DailyVector() {
		if( hours )
			delete[] hours ;
	}

	int parse(char *data){
		int offset=0;
		{ //Parsing vector hours
			size_t _length;
			offset += staticVarIntParse( data+offset , &_length );
		 	hoursSetLength( _length );
			for (int i = 0 ; i < (int)hours_length ; i++){
				offset += hours[i].parse(data+offset);
			}
		}
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		{ //Serialization vector hours
			offset += staticVarIntSerialize( data+offset , hours_length );
			for (int i = 0 ; i < (int)hours_length ; i++){
				offset += hours[i].serialize(data+offset);
			}
		}
		return offset;
	}

	int size(char *data){
		int offset=0;
		{ //Getting size of vector hours
			size_t _length;
			offset += staticVarIntParse( data+offset , &_length );
			MRData_TwoInt _tmp;
			for (int i = 0 ; i < (int)_length ; i++){
				offset += _tmp.size(data+offset);
			}
		}
		return offset;
	}

	int getPartition(int max_num_partitions){
		if( hours_length > 0 ){
		return hours[0].getPartition(max_num_partitions);
		} else return 0;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // Comparing vector hours
			size_t _length1,_length2;
			*offset1 += staticVarIntParse( data1+(*offset1) , &_length1 );
			*offset2 += staticVarIntParse( data2+(*offset2) , &_length2 );
			if( _length1 < _length2 ) return -1;
			if( _length1 > _length2 ) return 1;
			for (int i = 0 ; i < (int)_length1 ; i++){
				{ // comparing hours[i]
					MRData_TwoInt _tmp_data;
					int tmp = _tmp_data.compare(data1,data2,offset1 , offset2);
					if( tmp != 0) return tmp;
				}
			}
		}
		return 0; //If everything is equal
	}

	void hoursSetLength(int _length){
		if( _length > hours_max_length){ 
			MRData_TwoInt *_previous = hours;
			int previous_length = hours_length;
			if(hours_max_length == 0) hours_max_length = _length;
			while(hours_max_length < _length) hours_max_length *= 2;
			hours = new MRData_TwoInt[hours_max_length ];
			if( _previous ){
				for (int i = 0 ; i < previous_length ; i++)
					hours[i].copyFrom( &_previous[i] );
				delete[] _previous;
			}
		}
		hours_length=_length;
	}

	MRData_TwoInt* hoursAdd(){
		hoursSetLength( hours_length + 1 );
		return &hours[hours_length-1];
	}

	void copyFrom( MRData_DailyVector *other ){
			{ // CopyFrom field hours
				hoursSetLength( other->hours_length);
				for (int i = 0 ; i < hours_length ; i++){
					hours[i].copyFrom(&other->hours[i]);
				}
			}
	};
	std::string toString(){
		ostringstream o;
		{// toString of vector hours
			for(int i = 0 ; i < hours_length ; i++){
				o << hours[i].toString();
				 o << " ";
			}
		}
		o<<" ";
		return o.str();
	}

}; //class MRData_DailyVector

class MRData_ClusterVector : public MRData{

public:
	MRData_Double *coms;
	int coms_length;
	int coms_max_length;


	MRData_ClusterVector() : MRData(){
		coms_length=0;
		coms_max_length=0;
		coms = NULL;
	}

	~MRData_ClusterVector() {
		if( coms )
			delete[] coms ;
	}

	int parse(char *data){
		int offset=0;
		{ //Parsing vector coms
			size_t _length;
			offset += staticVarIntParse( data+offset , &_length );
		 	comsSetLength( _length );
			for (int i = 0 ; i < (int)coms_length ; i++){
				offset += coms[i].parse(data+offset);
			}
		}
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		{ //Serialization vector coms
			offset += staticVarIntSerialize( data+offset , coms_length );
			for (int i = 0 ; i < (int)coms_length ; i++){
				offset += coms[i].serialize(data+offset);
			}
		}
		return offset;
	}

	int size(char *data){
		int offset=0;
		{ //Getting size of vector coms
			size_t _length;
			offset += staticVarIntParse( data+offset , &_length );
			MRData_Double _tmp;
			for (int i = 0 ; i < (int)_length ; i++){
				offset += _tmp.size(data+offset);
			}
		}
		return offset;
	}

	int getPartition(int max_num_partitions){
		if( coms_length > 0 ){
		return coms[0].getPartition(max_num_partitions);
		} else return 0;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // Comparing vector coms
			size_t _length1,_length2;
			*offset1 += staticVarIntParse( data1+(*offset1) , &_length1 );
			*offset2 += staticVarIntParse( data2+(*offset2) , &_length2 );
			if( _length1 < _length2 ) return -1;
			if( _length1 > _length2 ) return 1;
			for (int i = 0 ; i < (int)_length1 ; i++){
				{ // comparing coms[i]
					MRData_Double _tmp_data;
					int tmp = _tmp_data.compare(data1,data2,offset1 , offset2);
					if( tmp != 0) return tmp;
				}
			}
		}
		return 0; //If everything is equal
	}

	void comsSetLength(int _length){
		if( _length > coms_max_length){ 
			MRData_Double *_previous = coms;
			int previous_length = coms_length;
			if(coms_max_length == 0) coms_max_length = _length;
			while(coms_max_length < _length) coms_max_length *= 2;
			coms = new MRData_Double[coms_max_length ];
			if( _previous ){
				for (int i = 0 ; i < previous_length ; i++)
					coms[i].copyFrom( &_previous[i] );
				delete[] _previous;
			}
		}
		coms_length=_length;
	}

	MRData_Double* comsAdd(){
		comsSetLength( coms_length + 1 );
		return &coms[coms_length-1];
	}

	void copyFrom( MRData_ClusterVector *other ){
			{ // CopyFrom field coms
				comsSetLength( other->coms_length);
				for (int i = 0 ; i < coms_length ; i++){
					coms[i].copyFrom(&other->coms[i]);
				}
			}
	};
	std::string toString(){
		ostringstream o;
		{// toString of vector coms
			for(int i = 0 ; i < coms_length ; i++){
				o << coms[i].toString();
				 o << " ";
			}
		}
		o<<" ";
		return o.str();
	}

}; //class MRData_ClusterVector

class MRData_Cluster : public MRData{

public:
	mr_uint8 label;
	mr_uint8 labelgroup;
	mr_uint8 confident;
	mr_double mean;
	mr_double distance;
	MRData_ClusterVector coords;


	MRData_Cluster() : MRData(){
	}

	~MRData_Cluster() {
	}

	int parse(char *data){
		int offset=0;
		label=  *( (mr_uint8*) (data+offset) ); offset +=  sizeof(mr_uint8);
		labelgroup=  *( (mr_uint8*) (data+offset) ); offset +=  sizeof(mr_uint8);
		confident=  *( (mr_uint8*) (data+offset) ); offset +=  sizeof(mr_uint8);
		mean=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		distance=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		offset += coords.parse(data+offset);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		*( (mr_uint8*) (data+offset) )=label; offset +=  sizeof(mr_uint8);
		*( (mr_uint8*) (data+offset) )=labelgroup; offset +=  sizeof(mr_uint8);
		*( (mr_uint8*) (data+offset) )=confident; offset +=  sizeof(mr_uint8);
		*( (mr_double*) (data+offset) )=mean; offset +=  sizeof(mr_double);
		*( (mr_double*) (data+offset) )=distance; offset +=  sizeof(mr_double);
		offset += coords.serialize(data+offset);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += sizeof(mr_uint8);
		offset += sizeof(mr_uint8);
		offset += sizeof(mr_uint8);
		offset += sizeof(mr_double);
		offset += sizeof(mr_double);
		offset += coords.size(data+offset);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return label%max_num_partitions;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // comparing label
			if  ( *((mr_uint8*)(data1 + (*offset1))) < *((mr_uint8*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_uint8*)(data1 + (*offset1))) > *((mr_uint8*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_uint8);
			*offset2 +=sizeof(mr_uint8);
		}
		{ // comparing labelgroup
			if  ( *((mr_uint8*)(data1 + (*offset1))) < *((mr_uint8*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_uint8*)(data1 + (*offset1))) > *((mr_uint8*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_uint8);
			*offset2 +=sizeof(mr_uint8);
		}
		{ // comparing confident
			if  ( *((mr_uint8*)(data1 + (*offset1))) < *((mr_uint8*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_uint8*)(data1 + (*offset1))) > *((mr_uint8*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_uint8);
			*offset2 +=sizeof(mr_uint8);
		}
		{ // comparing mean
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		{ // comparing distance
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		{ // comparing coords
			MRData_ClusterVector _tmp_data;
			int tmp = _tmp_data.compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		return 0; //If everything is equal
	}

	void copyFrom( MRData_Cluster *other ){
		label=other->label;
		labelgroup=other->labelgroup;
		confident=other->confident;
		mean=other->mean;
		distance=other->distance;
		coords.copyFrom(&other->coords);
	};
	std::string toString(){
		ostringstream o;
		o << (int)label;

		o<<" ";
		o << (int)labelgroup;

		o<<" ";
		o << (int)confident;

		o<<" ";
		o << mean;

		o<<" ";
		o << distance;

		o<<" ";
		o << coords.toString();

		o<<" ";
		return o.str();
	}

}; //class MRData_Cluster

class MRData_ClusterSet : public MRData{

public:
	MRData_Cluster *cluster;
	int cluster_length;
	int cluster_max_length;


	MRData_ClusterSet() : MRData(){
		cluster_length=0;
		cluster_max_length=0;
		cluster = NULL;
	}

	~MRData_ClusterSet() {
		if( cluster )
			delete[] cluster ;
	}

	int parse(char *data){
		int offset=0;
		{ //Parsing vector cluster
			size_t _length;
			offset += staticVarIntParse( data+offset , &_length );
		 	clusterSetLength( _length );
			for (int i = 0 ; i < (int)cluster_length ; i++){
				offset += cluster[i].parse(data+offset);
			}
		}
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		{ //Serialization vector cluster
			offset += staticVarIntSerialize( data+offset , cluster_length );
			for (int i = 0 ; i < (int)cluster_length ; i++){
				offset += cluster[i].serialize(data+offset);
			}
		}
		return offset;
	}

	int size(char *data){
		int offset=0;
		{ //Getting size of vector cluster
			size_t _length;
			offset += staticVarIntParse( data+offset , &_length );
			MRData_Cluster _tmp;
			for (int i = 0 ; i < (int)_length ; i++){
				offset += _tmp.size(data+offset);
			}
		}
		return offset;
	}

	int getPartition(int max_num_partitions){
		if( cluster_length > 0 ){
		return cluster[0].getPartition(max_num_partitions);
		} else return 0;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // Comparing vector cluster
			size_t _length1,_length2;
			*offset1 += staticVarIntParse( data1+(*offset1) , &_length1 );
			*offset2 += staticVarIntParse( data2+(*offset2) , &_length2 );
			if( _length1 < _length2 ) return -1;
			if( _length1 > _length2 ) return 1;
			for (int i = 0 ; i < (int)_length1 ; i++){
				{ // comparing cluster[i]
					MRData_Cluster _tmp_data;
					int tmp = _tmp_data.compare(data1,data2,offset1 , offset2);
					if( tmp != 0) return tmp;
				}
			}
		}
		return 0; //If everything is equal
	}

	void clusterSetLength(int _length){
		if( _length > cluster_max_length){ 
			MRData_Cluster *_previous = cluster;
			int previous_length = cluster_length;
			if(cluster_max_length == 0) cluster_max_length = _length;
			while(cluster_max_length < _length) cluster_max_length *= 2;
			cluster = new MRData_Cluster[cluster_max_length ];
			if( _previous ){
				for (int i = 0 ; i < previous_length ; i++)
					cluster[i].copyFrom( &_previous[i] );
				delete[] _previous;
			}
		}
		cluster_length=_length;
	}

	MRData_Cluster* clusterAdd(){
		clusterSetLength( cluster_length + 1 );
		return &cluster[cluster_length-1];
	}

	void copyFrom( MRData_ClusterSet *other ){
			{ // CopyFrom field cluster
				clusterSetLength( other->cluster_length);
				for (int i = 0 ; i < cluster_length ; i++){
					cluster[i].copyFrom(&other->cluster[i]);
				}
			}
	};
	std::string toString(){
		ostringstream o;
		{// toString of vector cluster
			for(int i = 0 ; i < cluster_length ; i++){
				o << cluster[i].toString();
				 o << " ";
			}
		}
		o<<" ";
		return o.str();
	}

}; //class MRData_ClusterSet

class MRData_PoiPos : public MRData{

public:
	mr_uint node;
	mr_uint bts;
	mr_uint label;
	mr_double posx;
	mr_double posy;
	mr_int8 inoutWeek;
	mr_int8 inoutWend;
	mr_double radiusWeek;
	mr_double distCMWeek;
	mr_double radiusWend;
	mr_double distCMWend;


	MRData_PoiPos() : MRData(){
	}

	~MRData_PoiPos() {
	}

	int parse(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &node);
		offset += staticVarIntParse( data+offset , &bts);
		offset += staticVarIntParse( data+offset , &label);
		posx=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		posy=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		inoutWeek=  *( (mr_int8*) (data+offset) ); offset +=  sizeof(mr_int8);
		inoutWend=  *( (mr_int8*) (data+offset) ); offset +=  sizeof(mr_int8);
		radiusWeek=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		distCMWeek=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		radiusWend=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		distCMWend=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += staticVarIntSerialize( data+offset , node);
		offset += staticVarIntSerialize( data+offset , bts);
		offset += staticVarIntSerialize( data+offset , label);
		*( (mr_double*) (data+offset) )=posx; offset +=  sizeof(mr_double);
		*( (mr_double*) (data+offset) )=posy; offset +=  sizeof(mr_double);
		*( (mr_int8*) (data+offset) )=inoutWeek; offset +=  sizeof(mr_int8);
		*( (mr_int8*) (data+offset) )=inoutWend; offset +=  sizeof(mr_int8);
		*( (mr_double*) (data+offset) )=radiusWeek; offset +=  sizeof(mr_double);
		*( (mr_double*) (data+offset) )=distCMWeek; offset +=  sizeof(mr_double);
		*( (mr_double*) (data+offset) )=radiusWend; offset +=  sizeof(mr_double);
		*( (mr_double*) (data+offset) )=distCMWend; offset +=  sizeof(mr_double);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &node);
		offset += staticVarIntParse( data+offset , &bts);
		offset += staticVarIntParse( data+offset , &label);
		offset += sizeof(mr_double);
		offset += sizeof(mr_double);
		offset += sizeof(mr_int8);
		offset += sizeof(mr_int8);
		offset += sizeof(mr_double);
		offset += sizeof(mr_double);
		offset += sizeof(mr_double);
		offset += sizeof(mr_double);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return node%max_num_partitions;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // comparing node
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing bts
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing label
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing posx
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		{ // comparing posy
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		{ // comparing inoutWeek
			if  ( *((mr_int8*)(data1 + (*offset1))) < *((mr_int8*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_int8*)(data1 + (*offset1))) > *((mr_int8*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_int8);
			*offset2 +=sizeof(mr_int8);
		}
		{ // comparing inoutWend
			if  ( *((mr_int8*)(data1 + (*offset1))) < *((mr_int8*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_int8*)(data1 + (*offset1))) > *((mr_int8*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_int8);
			*offset2 +=sizeof(mr_int8);
		}
		{ // comparing radiusWeek
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		{ // comparing distCMWeek
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		{ // comparing radiusWend
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		{ // comparing distCMWend
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		return 0; //If everything is equal
	}

	void copyFrom( MRData_PoiPos *other ){
		node=other->node;
		bts=other->bts;
		label=other->label;
		posx=other->posx;
		posy=other->posy;
		inoutWeek=other->inoutWeek;
		inoutWend=other->inoutWend;
		radiusWeek=other->radiusWeek;
		distCMWeek=other->distCMWeek;
		radiusWend=other->radiusWend;
		distCMWend=other->distCMWend;
	};
	std::string toString(){
		ostringstream o;
		o << node;

		o<<" ";
		o << bts;

		o<<" ";
		o << label;

		o<<" ";
		o << posx;

		o<<" ";
		o << posy;

		o<<" ";
		o << (int)inoutWeek;

		o<<" ";
		o << (int)inoutWend;

		o<<" ";
		o << radiusWeek;

		o<<" ";
		o << distCMWeek;

		o<<" ";
		o << radiusWend;

		o<<" ";
		o << distCMWend;

		o<<" ";
		return o.str();
	}

}; //class MRData_PoiPos

class MRData_PoiNew : public MRData{

public:
	mr_uint id;
	mr_uint node;
	mr_uint bts;
	mr_uint labelgroupnodebts;
	mr_uint8 confidentnodebts;


	MRData_PoiNew() : MRData(){
	}

	~MRData_PoiNew() {
	}

	int parse(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &id);
		offset += staticVarIntParse( data+offset , &node);
		offset += staticVarIntParse( data+offset , &bts);
		offset += staticVarIntParse( data+offset , &labelgroupnodebts);
		confidentnodebts=  *( (mr_uint8*) (data+offset) ); offset +=  sizeof(mr_uint8);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += staticVarIntSerialize( data+offset , id);
		offset += staticVarIntSerialize( data+offset , node);
		offset += staticVarIntSerialize( data+offset , bts);
		offset += staticVarIntSerialize( data+offset , labelgroupnodebts);
		*( (mr_uint8*) (data+offset) )=confidentnodebts; offset +=  sizeof(mr_uint8);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &id);
		offset += staticVarIntParse( data+offset , &node);
		offset += staticVarIntParse( data+offset , &bts);
		offset += staticVarIntParse( data+offset , &labelgroupnodebts);
		offset += sizeof(mr_uint8);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return id%max_num_partitions;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // comparing id
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing node
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing bts
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing labelgroupnodebts
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing confidentnodebts
			if  ( *((mr_uint8*)(data1 + (*offset1))) < *((mr_uint8*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_uint8*)(data1 + (*offset1))) > *((mr_uint8*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_uint8);
			*offset2 +=sizeof(mr_uint8);
		}
		return 0; //If everything is equal
	}

	void copyFrom( MRData_PoiNew *other ){
		id=other->id;
		node=other->node;
		bts=other->bts;
		labelgroupnodebts=other->labelgroupnodebts;
		confidentnodebts=other->confidentnodebts;
	};
	std::string toString(){
		ostringstream o;
		o << id;

		o<<" ";
		o << node;

		o<<" ";
		o << bts;

		o<<" ";
		o << labelgroupnodebts;

		o<<" ";
		o << (int)confidentnodebts;

		o<<" ";
		return o.str();
	}

}; //class MRData_PoiNew

class MRData_PoiAdj : public MRData{

public:
	mr_uint node;
	mr_uint bts1;
	mr_uint bts2;
	mr_uint poi1;
	mr_uint poi2;
	mr_uint lblpoi;


	MRData_PoiAdj() : MRData(){
	}

	~MRData_PoiAdj() {
	}

	int parse(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &node);
		offset += staticVarIntParse( data+offset , &bts1);
		offset += staticVarIntParse( data+offset , &bts2);
		offset += staticVarIntParse( data+offset , &poi1);
		offset += staticVarIntParse( data+offset , &poi2);
		offset += staticVarIntParse( data+offset , &lblpoi);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += staticVarIntSerialize( data+offset , node);
		offset += staticVarIntSerialize( data+offset , bts1);
		offset += staticVarIntSerialize( data+offset , bts2);
		offset += staticVarIntSerialize( data+offset , poi1);
		offset += staticVarIntSerialize( data+offset , poi2);
		offset += staticVarIntSerialize( data+offset , lblpoi);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &node);
		offset += staticVarIntParse( data+offset , &bts1);
		offset += staticVarIntParse( data+offset , &bts2);
		offset += staticVarIntParse( data+offset , &poi1);
		offset += staticVarIntParse( data+offset , &poi2);
		offset += staticVarIntParse( data+offset , &lblpoi);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return node%max_num_partitions;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // comparing node
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing bts1
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing bts2
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing poi1
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing poi2
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing lblpoi
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		return 0; //If everything is equal
	}

	void copyFrom( MRData_PoiAdj *other ){
		node=other->node;
		bts1=other->bts1;
		bts2=other->bts2;
		poi1=other->poi1;
		poi2=other->poi2;
		lblpoi=other->lblpoi;
	};
	std::string toString(){
		ostringstream o;
		o << node;

		o<<" ";
		o << bts1;

		o<<" ";
		o << bts2;

		o<<" ";
		o << poi1;

		o<<" ";
		o << poi2;

		o<<" ";
		o << lblpoi;

		o<<" ";
		return o.str();
	}

}; //class MRData_PoiAdj

class MRData_VectorPairs : public MRData{

public:
	MRData_TwoInt *values;
	int values_length;
	int values_max_length;


	MRData_VectorPairs() : MRData(){
		values_length=0;
		values_max_length=0;
		values = NULL;
	}

	~MRData_VectorPairs() {
		if( values )
			delete[] values ;
	}

	int parse(char *data){
		int offset=0;
		{ //Parsing vector values
			size_t _length;
			offset += staticVarIntParse( data+offset , &_length );
		 	valuesSetLength( _length );
			for (int i = 0 ; i < (int)values_length ; i++){
				offset += values[i].parse(data+offset);
			}
		}
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		{ //Serialization vector values
			offset += staticVarIntSerialize( data+offset , values_length );
			for (int i = 0 ; i < (int)values_length ; i++){
				offset += values[i].serialize(data+offset);
			}
		}
		return offset;
	}

	int size(char *data){
		int offset=0;
		{ //Getting size of vector values
			size_t _length;
			offset += staticVarIntParse( data+offset , &_length );
			MRData_TwoInt _tmp;
			for (int i = 0 ; i < (int)_length ; i++){
				offset += _tmp.size(data+offset);
			}
		}
		return offset;
	}

	int getPartition(int max_num_partitions){
		if( values_length > 0 ){
		return values[0].getPartition(max_num_partitions);
		} else return 0;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // Comparing vector values
			size_t _length1,_length2;
			*offset1 += staticVarIntParse( data1+(*offset1) , &_length1 );
			*offset2 += staticVarIntParse( data2+(*offset2) , &_length2 );
			if( _length1 < _length2 ) return -1;
			if( _length1 > _length2 ) return 1;
			for (int i = 0 ; i < (int)_length1 ; i++){
				{ // comparing values[i]
					MRData_TwoInt _tmp_data;
					int tmp = _tmp_data.compare(data1,data2,offset1 , offset2);
					if( tmp != 0) return tmp;
				}
			}
		}
		return 0; //If everything is equal
	}

	void valuesSetLength(int _length){
		if( _length > values_max_length){ 
			MRData_TwoInt *_previous = values;
			int previous_length = values_length;
			if(values_max_length == 0) values_max_length = _length;
			while(values_max_length < _length) values_max_length *= 2;
			values = new MRData_TwoInt[values_max_length ];
			if( _previous ){
				for (int i = 0 ; i < previous_length ; i++)
					values[i].copyFrom( &_previous[i] );
				delete[] _previous;
			}
		}
		values_length=_length;
	}

	MRData_TwoInt* valuesAdd(){
		valuesSetLength( values_length + 1 );
		return &values[values_length-1];
	}

	void copyFrom( MRData_VectorPairs *other ){
			{ // CopyFrom field values
				valuesSetLength( other->values_length);
				for (int i = 0 ; i < values_length ; i++){
					values[i].copyFrom(&other->values[i]);
				}
			}
	};
	std::string toString(){
		ostringstream o;
		{// toString of vector values
			for(int i = 0 ; i < values_length ; i++){
				o << values[i].toString();
				 o << " ";
			}
		}
		o<<" ";
		return o.str();
	}

}; //class MRData_VectorPairs

class MRData_TelMonth : public MRData{

public:
	mr_uint phone;
	mr_uint month;
	mr_uint workingday;


	MRData_TelMonth() : MRData(){
	}

	~MRData_TelMonth() {
	}

	int parse(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &phone);
		offset += staticVarIntParse( data+offset , &month);
		offset += staticVarIntParse( data+offset , &workingday);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += staticVarIntSerialize( data+offset , phone);
		offset += staticVarIntSerialize( data+offset , month);
		offset += staticVarIntSerialize( data+offset , workingday);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &phone);
		offset += staticVarIntParse( data+offset , &month);
		offset += staticVarIntParse( data+offset , &workingday);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return phone%max_num_partitions;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // comparing phone
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing month
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing workingday
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		return 0; //If everything is equal
	}

	void copyFrom( MRData_TelMonth *other ){
		phone=other->phone;
		month=other->month;
		workingday=other->workingday;
	};
	std::string toString(){
		ostringstream o;
		o << phone;

		o<<" ";
		o << month;

		o<<" ";
		o << workingday;

		o<<" ";
		return o.str();
	}

}; //class MRData_TelMonth

class MRData_TempMob : public MRData{

public:
	mr_uint *btss;
	int btss_length;
	int btss_max_length;
	mr_uint *muns;
	int muns_length;
	int muns_max_length;
	mr_uint *states;
	int states_length;
	int states_max_length;
	mr_double *coord_utmx;
	int coord_utmx_length;
	int coord_utmx_max_length;
	mr_double *coord_utmy;
	int coord_utmy_length;
	int coord_utmy_max_length;


	MRData_TempMob() : MRData(){
		btss_length=0;
		btss_max_length=0;
		btss = NULL;
		muns_length=0;
		muns_max_length=0;
		muns = NULL;
		states_length=0;
		states_max_length=0;
		states = NULL;
		coord_utmx_length=0;
		coord_utmx_max_length=0;
		coord_utmx = NULL;
		coord_utmy_length=0;
		coord_utmy_max_length=0;
		coord_utmy = NULL;
	}

	~MRData_TempMob() {
		if( btss )
			free( btss );
		if( muns )
			free( muns );
		if( states )
			free( states );
		if( coord_utmx )
			free( coord_utmx );
		if( coord_utmy )
			free( coord_utmy );
	}

	int parse(char *data){
		int offset=0;
		{ //Parsing vector btss
			size_t _length;
			offset += staticVarIntParse( data+offset , &_length );
		 	btssSetLength( _length );
			for (int i = 0 ; i < (int)btss_length ; i++){
				offset += staticVarIntParse( data+offset , &btss[i]);
			}
		}
		{ //Parsing vector muns
			size_t _length;
			offset += staticVarIntParse( data+offset , &_length );
		 	munsSetLength( _length );
			for (int i = 0 ; i < (int)muns_length ; i++){
				offset += staticVarIntParse( data+offset , &muns[i]);
			}
		}
		{ //Parsing vector states
			size_t _length;
			offset += staticVarIntParse( data+offset , &_length );
		 	statesSetLength( _length );
			for (int i = 0 ; i < (int)states_length ; i++){
				offset += staticVarIntParse( data+offset , &states[i]);
			}
		}
		{ //Parsing vector coord_utmx
			size_t _length;
			offset += staticVarIntParse( data+offset , &_length );
		 	coord_utmxSetLength( _length );
			for (int i = 0 ; i < (int)coord_utmx_length ; i++){
				coord_utmx[i]=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
			}
		}
		{ //Parsing vector coord_utmy
			size_t _length;
			offset += staticVarIntParse( data+offset , &_length );
		 	coord_utmySetLength( _length );
			for (int i = 0 ; i < (int)coord_utmy_length ; i++){
				coord_utmy[i]=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
			}
		}
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		{ //Serialization vector btss
			offset += staticVarIntSerialize( data+offset , btss_length );
			for (int i = 0 ; i < (int)btss_length ; i++){
				offset += staticVarIntSerialize( data+offset , btss[i]);
			}
		}
		{ //Serialization vector muns
			offset += staticVarIntSerialize( data+offset , muns_length );
			for (int i = 0 ; i < (int)muns_length ; i++){
				offset += staticVarIntSerialize( data+offset , muns[i]);
			}
		}
		{ //Serialization vector states
			offset += staticVarIntSerialize( data+offset , states_length );
			for (int i = 0 ; i < (int)states_length ; i++){
				offset += staticVarIntSerialize( data+offset , states[i]);
			}
		}
		{ //Serialization vector coord_utmx
			offset += staticVarIntSerialize( data+offset , coord_utmx_length );
			for (int i = 0 ; i < (int)coord_utmx_length ; i++){
				*( (mr_double*) (data+offset) )=coord_utmx[i]; offset +=  sizeof(mr_double);
			}
		}
		{ //Serialization vector coord_utmy
			offset += staticVarIntSerialize( data+offset , coord_utmy_length );
			for (int i = 0 ; i < (int)coord_utmy_length ; i++){
				*( (mr_double*) (data+offset) )=coord_utmy[i]; offset +=  sizeof(mr_double);
			}
		}
		return offset;
	}

	int size(char *data){
		int offset=0;
		{ //Getting size of vector btss
			size_t _length;
			offset += staticVarIntParse( data+offset , &_length );
			mr_uint _tmp;
			for (int i = 0 ; i < (int)_length ; i++){
				offset += staticVarIntParse( data+offset , &_tmp);
			}
		}
		{ //Getting size of vector muns
			size_t _length;
			offset += staticVarIntParse( data+offset , &_length );
			mr_uint _tmp;
			for (int i = 0 ; i < (int)_length ; i++){
				offset += staticVarIntParse( data+offset , &_tmp);
			}
		}
		{ //Getting size of vector states
			size_t _length;
			offset += staticVarIntParse( data+offset , &_length );
			mr_uint _tmp;
			for (int i = 0 ; i < (int)_length ; i++){
				offset += staticVarIntParse( data+offset , &_tmp);
			}
		}
		{ //Getting size of vector coord_utmx
			size_t _length;
			offset += staticVarIntParse( data+offset , &_length );
			offset += _length*sizeof(mr_double);
		}
		{ //Getting size of vector coord_utmy
			size_t _length;
			offset += staticVarIntParse( data+offset , &_length );
			offset += _length*sizeof(mr_double);
		}
		return offset;
	}

	int getPartition(int max_num_partitions){
		if( btss_length > 0 ){
		return btss[0]%max_num_partitions;
		} else return 0;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // Comparing vector btss
			size_t _length1,_length2;
			*offset1 += staticVarIntParse( data1+(*offset1) , &_length1 );
			*offset2 += staticVarIntParse( data2+(*offset2) , &_length2 );
			if( _length1 < _length2 ) return -1;
			if( _length1 > _length2 ) return 1;
			for (int i = 0 ; i < (int)_length1 ; i++){
				{ // comparing btss[i]
					mr_uint _value1;
					mr_uint _value2;
					*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
					*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
					if( _value1 < _value2 ) return -1;
					if( _value1 > _value2 ) return  1;
				}
			}
		}
		{ // Comparing vector muns
			size_t _length1,_length2;
			*offset1 += staticVarIntParse( data1+(*offset1) , &_length1 );
			*offset2 += staticVarIntParse( data2+(*offset2) , &_length2 );
			if( _length1 < _length2 ) return -1;
			if( _length1 > _length2 ) return 1;
			for (int i = 0 ; i < (int)_length1 ; i++){
				{ // comparing muns[i]
					mr_uint _value1;
					mr_uint _value2;
					*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
					*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
					if( _value1 < _value2 ) return -1;
					if( _value1 > _value2 ) return  1;
				}
			}
		}
		{ // Comparing vector states
			size_t _length1,_length2;
			*offset1 += staticVarIntParse( data1+(*offset1) , &_length1 );
			*offset2 += staticVarIntParse( data2+(*offset2) , &_length2 );
			if( _length1 < _length2 ) return -1;
			if( _length1 > _length2 ) return 1;
			for (int i = 0 ; i < (int)_length1 ; i++){
				{ // comparing states[i]
					mr_uint _value1;
					mr_uint _value2;
					*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
					*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
					if( _value1 < _value2 ) return -1;
					if( _value1 > _value2 ) return  1;
				}
			}
		}
		{ // Comparing vector coord_utmx
			size_t _length1,_length2;
			*offset1 += staticVarIntParse( data1+(*offset1) , &_length1 );
			*offset2 += staticVarIntParse( data2+(*offset2) , &_length2 );
			if( _length1 < _length2 ) return -1;
			if( _length1 > _length2 ) return 1;
			for (int i = 0 ; i < (int)_length1 ; i++){
				{ // comparing coord_utmx[i]
					if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
					if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
					*offset1 +=sizeof(mr_double);
					*offset2 +=sizeof(mr_double);
				}
			}
		}
		{ // Comparing vector coord_utmy
			size_t _length1,_length2;
			*offset1 += staticVarIntParse( data1+(*offset1) , &_length1 );
			*offset2 += staticVarIntParse( data2+(*offset2) , &_length2 );
			if( _length1 < _length2 ) return -1;
			if( _length1 > _length2 ) return 1;
			for (int i = 0 ; i < (int)_length1 ; i++){
				{ // comparing coord_utmy[i]
					if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
					if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
					*offset1 +=sizeof(mr_double);
					*offset2 +=sizeof(mr_double);
				}
			}
		}
		return 0; //If everything is equal
	}

	void btssSetLength(int _length){
		if( _length > btss_max_length){ 
			mr_uint *_previous = btss;
			int previous_length = btss_length;
			if(btss_max_length == 0) btss_max_length = _length;
			while(btss_max_length < _length) btss_max_length *= 2;
			btss = (mr_uint*) malloc( sizeof(mr_uint) * btss_max_length );
			if( _previous ){
				memcpy(btss, _previous, previous_length*sizeof(mr_uint));
				free( _previous );
			}
		}
		btss_length=_length;
	}

	void btssAdd(mr_uint _value){
		btssSetLength( btss_length + 1 );
		btss[btss_length-1] = _value;
	}

	void munsSetLength(int _length){
		if( _length > muns_max_length){ 
			mr_uint *_previous = muns;
			int previous_length = muns_length;
			if(muns_max_length == 0) muns_max_length = _length;
			while(muns_max_length < _length) muns_max_length *= 2;
			muns = (mr_uint*) malloc( sizeof(mr_uint) * muns_max_length );
			if( _previous ){
				memcpy(muns, _previous, previous_length*sizeof(mr_uint));
				free( _previous );
			}
		}
		muns_length=_length;
	}

	void munsAdd(mr_uint _value){
		munsSetLength( muns_length + 1 );
		muns[muns_length-1] = _value;
	}

	void statesSetLength(int _length){
		if( _length > states_max_length){ 
			mr_uint *_previous = states;
			int previous_length = states_length;
			if(states_max_length == 0) states_max_length = _length;
			while(states_max_length < _length) states_max_length *= 2;
			states = (mr_uint*) malloc( sizeof(mr_uint) * states_max_length );
			if( _previous ){
				memcpy(states, _previous, previous_length*sizeof(mr_uint));
				free( _previous );
			}
		}
		states_length=_length;
	}

	void statesAdd(mr_uint _value){
		statesSetLength( states_length + 1 );
		states[states_length-1] = _value;
	}

	void coord_utmxSetLength(int _length){
		if( _length > coord_utmx_max_length){ 
			mr_double *_previous = coord_utmx;
			int previous_length = coord_utmx_length;
			if(coord_utmx_max_length == 0) coord_utmx_max_length = _length;
			while(coord_utmx_max_length < _length) coord_utmx_max_length *= 2;
			coord_utmx = (mr_double*) malloc( sizeof(mr_double) * coord_utmx_max_length );
			if( _previous ){
				memcpy(coord_utmx, _previous, previous_length*sizeof(mr_double));
				free( _previous );
			}
		}
		coord_utmx_length=_length;
	}

	void coord_utmxAdd(mr_double _value){
		coord_utmxSetLength( coord_utmx_length + 1 );
		coord_utmx[coord_utmx_length-1] = _value;
	}

	void coord_utmySetLength(int _length){
		if( _length > coord_utmy_max_length){ 
			mr_double *_previous = coord_utmy;
			int previous_length = coord_utmy_length;
			if(coord_utmy_max_length == 0) coord_utmy_max_length = _length;
			while(coord_utmy_max_length < _length) coord_utmy_max_length *= 2;
			coord_utmy = (mr_double*) malloc( sizeof(mr_double) * coord_utmy_max_length );
			if( _previous ){
				memcpy(coord_utmy, _previous, previous_length*sizeof(mr_double));
				free( _previous );
			}
		}
		coord_utmy_length=_length;
	}

	void coord_utmyAdd(mr_double _value){
		coord_utmySetLength( coord_utmy_length + 1 );
		coord_utmy[coord_utmy_length-1] = _value;
	}

	void copyFrom( MRData_TempMob *other ){
			{ // CopyFrom field btss
				btssSetLength( other->btss_length);
				for (int i = 0 ; i < btss_length ; i++){
					btss[i]=other->btss[i];
				}
			}
			{ // CopyFrom field muns
				munsSetLength( other->muns_length);
				for (int i = 0 ; i < muns_length ; i++){
					muns[i]=other->muns[i];
				}
			}
			{ // CopyFrom field states
				statesSetLength( other->states_length);
				for (int i = 0 ; i < states_length ; i++){
					states[i]=other->states[i];
				}
			}
			{ // CopyFrom field coord_utmx
				coord_utmxSetLength( other->coord_utmx_length);
				for (int i = 0 ; i < coord_utmx_length ; i++){
					coord_utmx[i]=other->coord_utmx[i];
				}
			}
			{ // CopyFrom field coord_utmy
				coord_utmySetLength( other->coord_utmy_length);
				for (int i = 0 ; i < coord_utmy_length ; i++){
					coord_utmy[i]=other->coord_utmy[i];
				}
			}
	};
	std::string toString(){
		ostringstream o;
		{// toString of vector btss
			for(int i = 0 ; i < btss_length ; i++){
				o << btss[i];
				 o << " ";
			}
		}
		o<<" ";
		{// toString of vector muns
			for(int i = 0 ; i < muns_length ; i++){
				o << muns[i];
				 o << " ";
			}
		}
		o<<" ";
		{// toString of vector states
			for(int i = 0 ; i < states_length ; i++){
				o << states[i];
				 o << " ";
			}
		}
		o<<" ";
		{// toString of vector coord_utmx
			for(int i = 0 ; i < coord_utmx_length ; i++){
				o << coord_utmx[i];
				 o << " ";
			}
		}
		o<<" ";
		{// toString of vector coord_utmy
			for(int i = 0 ; i < coord_utmy_length ; i++){
				o << coord_utmy[i];
				 o << " ";
			}
		}
		o<<" ";
		return o.str();
	}

}; //class MRData_TempMob

class MRData_MobVars : public MRData{

public:
	mr_uint month;
	mr_uint workingday;
	mr_uint num_pos;
	mr_uint dif_btss;
	mr_uint dif_muns;
	mr_uint dif_states;
	mr_double masscenter_utmx;
	mr_double masscenter_utmy;
	mr_double radius;
	mr_double diam_areainf;


	MRData_MobVars() : MRData(){
	}

	~MRData_MobVars() {
	}

	int parse(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &month);
		offset += staticVarIntParse( data+offset , &workingday);
		offset += staticVarIntParse( data+offset , &num_pos);
		offset += staticVarIntParse( data+offset , &dif_btss);
		offset += staticVarIntParse( data+offset , &dif_muns);
		offset += staticVarIntParse( data+offset , &dif_states);
		masscenter_utmx=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		masscenter_utmy=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		radius=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		diam_areainf=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += staticVarIntSerialize( data+offset , month);
		offset += staticVarIntSerialize( data+offset , workingday);
		offset += staticVarIntSerialize( data+offset , num_pos);
		offset += staticVarIntSerialize( data+offset , dif_btss);
		offset += staticVarIntSerialize( data+offset , dif_muns);
		offset += staticVarIntSerialize( data+offset , dif_states);
		*( (mr_double*) (data+offset) )=masscenter_utmx; offset +=  sizeof(mr_double);
		*( (mr_double*) (data+offset) )=masscenter_utmy; offset +=  sizeof(mr_double);
		*( (mr_double*) (data+offset) )=radius; offset +=  sizeof(mr_double);
		*( (mr_double*) (data+offset) )=diam_areainf; offset +=  sizeof(mr_double);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &month);
		offset += staticVarIntParse( data+offset , &workingday);
		offset += staticVarIntParse( data+offset , &num_pos);
		offset += staticVarIntParse( data+offset , &dif_btss);
		offset += staticVarIntParse( data+offset , &dif_muns);
		offset += staticVarIntParse( data+offset , &dif_states);
		offset += sizeof(mr_double);
		offset += sizeof(mr_double);
		offset += sizeof(mr_double);
		offset += sizeof(mr_double);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return month%max_num_partitions;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // comparing month
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing workingday
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing num_pos
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing dif_btss
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing dif_muns
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing dif_states
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing masscenter_utmx
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		{ // comparing masscenter_utmy
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		{ // comparing radius
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		{ // comparing diam_areainf
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		return 0; //If everything is equal
	}

	void copyFrom( MRData_MobVars *other ){
		month=other->month;
		workingday=other->workingday;
		num_pos=other->num_pos;
		dif_btss=other->dif_btss;
		dif_muns=other->dif_muns;
		dif_states=other->dif_states;
		masscenter_utmx=other->masscenter_utmx;
		masscenter_utmy=other->masscenter_utmy;
		radius=other->radius;
		diam_areainf=other->diam_areainf;
	};
	std::string toString(){
		ostringstream o;
		o << month;

		o<<" ";
		o << workingday;

		o<<" ";
		o << num_pos;

		o<<" ";
		o << dif_btss;

		o<<" ";
		o << dif_muns;

		o<<" ";
		o << dif_states;

		o<<" ";
		o << masscenter_utmx;

		o<<" ";
		o << masscenter_utmy;

		o<<" ";
		o << radius;

		o<<" ";
		o << diam_areainf;

		o<<" ";
		return o.str();
	}

}; //class MRData_MobVars

class MRData_Mob_VI_MobVars : public MRData{

public:
	MRData_MobVars *vars;
	int vars_length;
	int vars_max_length;


	MRData_Mob_VI_MobVars() : MRData(){
		vars_length=0;
		vars_max_length=0;
		vars = NULL;
	}

	~MRData_Mob_VI_MobVars() {
		if( vars )
			delete[] vars ;
	}

	int parse(char *data){
		int offset=0;
		{ //Parsing vector vars
			size_t _length;
			offset += staticVarIntParse( data+offset , &_length );
		 	varsSetLength( _length );
			for (int i = 0 ; i < (int)vars_length ; i++){
				offset += vars[i].parse(data+offset);
			}
		}
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		{ //Serialization vector vars
			offset += staticVarIntSerialize( data+offset , vars_length );
			for (int i = 0 ; i < (int)vars_length ; i++){
				offset += vars[i].serialize(data+offset);
			}
		}
		return offset;
	}

	int size(char *data){
		int offset=0;
		{ //Getting size of vector vars
			size_t _length;
			offset += staticVarIntParse( data+offset , &_length );
			MRData_MobVars _tmp;
			for (int i = 0 ; i < (int)_length ; i++){
				offset += _tmp.size(data+offset);
			}
		}
		return offset;
	}

	int getPartition(int max_num_partitions){
		if( vars_length > 0 ){
		return vars[0].getPartition(max_num_partitions);
		} else return 0;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // Comparing vector vars
			size_t _length1,_length2;
			*offset1 += staticVarIntParse( data1+(*offset1) , &_length1 );
			*offset2 += staticVarIntParse( data2+(*offset2) , &_length2 );
			if( _length1 < _length2 ) return -1;
			if( _length1 > _length2 ) return 1;
			for (int i = 0 ; i < (int)_length1 ; i++){
				{ // comparing vars[i]
					MRData_MobVars _tmp_data;
					int tmp = _tmp_data.compare(data1,data2,offset1 , offset2);
					if( tmp != 0) return tmp;
				}
			}
		}
		return 0; //If everything is equal
	}

	void varsSetLength(int _length){
		if( _length > vars_max_length){ 
			MRData_MobVars *_previous = vars;
			int previous_length = vars_length;
			if(vars_max_length == 0) vars_max_length = _length;
			while(vars_max_length < _length) vars_max_length *= 2;
			vars = new MRData_MobVars[vars_max_length ];
			if( _previous ){
				for (int i = 0 ; i < previous_length ; i++)
					vars[i].copyFrom( &_previous[i] );
				delete[] _previous;
			}
		}
		vars_length=_length;
	}

	MRData_MobVars* varsAdd(){
		varsSetLength( vars_length + 1 );
		return &vars[vars_length-1];
	}

	void copyFrom( MRData_Mob_VI_MobVars *other ){
			{ // CopyFrom field vars
				varsSetLength( other->vars_length);
				for (int i = 0 ; i < vars_length ; i++){
					vars[i].copyFrom(&other->vars[i]);
				}
			}
	};
	std::string toString(){
		ostringstream o;
		{// toString of vector vars
			for(int i = 0 ; i < vars_length ; i++){
				o << vars[i].toString();
				 o << " ";
			}
		}
		o<<" ";
		return o.str();
	}

}; //class MRData_Mob_VI_MobVars

class MRData_ItinTime : public MRData{

public:
	MRData_Date date;
	MRData_Time time;
	mr_uint bts;


	MRData_ItinTime() : MRData(){
	}

	~MRData_ItinTime() {
	}

	int parse(char *data){
		int offset=0;
		offset += date.parse(data+offset);
		offset += time.parse(data+offset);
		offset += staticVarIntParse( data+offset , &bts);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += date.serialize(data+offset);
		offset += time.serialize(data+offset);
		offset += staticVarIntSerialize( data+offset , bts);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += date.size(data+offset);
		offset += time.size(data+offset);
		offset += staticVarIntParse( data+offset , &bts);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return date.getPartition(max_num_partitions);
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // comparing date
			MRData_Date _tmp_data;
			int tmp = _tmp_data.compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing time
			MRData_Time _tmp_data;
			int tmp = _tmp_data.compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing bts
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		return 0; //If everything is equal
	}

	void copyFrom( MRData_ItinTime *other ){
		date.copyFrom(&other->date);
		time.copyFrom(&other->time);
		bts=other->bts;
	};
	std::string toString(){
		ostringstream o;
		o << date.toString();

		o<<" ";
		o << time.toString();

		o<<" ";
		o << bts;

		o<<" ";
		return o.str();
	}

}; //class MRData_ItinTime

class MRData_ItinMovement : public MRData{

public:
	MRData_ItinTime source;
	MRData_ItinTime target;


	MRData_ItinMovement() : MRData(){
	}

	~MRData_ItinMovement() {
	}

	int parse(char *data){
		int offset=0;
		offset += source.parse(data+offset);
		offset += target.parse(data+offset);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += source.serialize(data+offset);
		offset += target.serialize(data+offset);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += source.size(data+offset);
		offset += target.size(data+offset);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return source.getPartition(max_num_partitions);
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // comparing source
			MRData_ItinTime _tmp_data;
			int tmp = _tmp_data.compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing target
			MRData_ItinTime _tmp_data;
			int tmp = _tmp_data.compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		return 0; //If everything is equal
	}

	void copyFrom( MRData_ItinMovement *other ){
		source.copyFrom(&other->source);
		target.copyFrom(&other->target);
	};
	std::string toString(){
		ostringstream o;
		o << source.toString();

		o<<" ";
		o << target.toString();

		o<<" ";
		return o.str();
	}

}; //class MRData_ItinMovement

class MRData_ItinRange : public MRData{

public:
	mr_uint poiSrc;
	mr_uint poiTgt;
	mr_uint node;
	mr_uint group;
	mr_uint range;


	MRData_ItinRange() : MRData(){
	}

	~MRData_ItinRange() {
	}

	int parse(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &poiSrc);
		offset += staticVarIntParse( data+offset , &poiTgt);
		offset += staticVarIntParse( data+offset , &node);
		offset += staticVarIntParse( data+offset , &group);
		offset += staticVarIntParse( data+offset , &range);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += staticVarIntSerialize( data+offset , poiSrc);
		offset += staticVarIntSerialize( data+offset , poiTgt);
		offset += staticVarIntSerialize( data+offset , node);
		offset += staticVarIntSerialize( data+offset , group);
		offset += staticVarIntSerialize( data+offset , range);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &poiSrc);
		offset += staticVarIntParse( data+offset , &poiTgt);
		offset += staticVarIntParse( data+offset , &node);
		offset += staticVarIntParse( data+offset , &group);
		offset += staticVarIntParse( data+offset , &range);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return poiSrc%max_num_partitions;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // comparing poiSrc
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing poiTgt
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing node
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing group
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing range
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		return 0; //If everything is equal
	}

	void copyFrom( MRData_ItinRange *other ){
		poiSrc=other->poiSrc;
		poiTgt=other->poiTgt;
		node=other->node;
		group=other->group;
		range=other->range;
	};
	std::string toString(){
		ostringstream o;
		o << poiSrc;

		o<<" ";
		o << poiTgt;

		o<<" ";
		o << node;

		o<<" ";
		o << group;

		o<<" ";
		o << range;

		o<<" ";
		return o.str();
	}

}; //class MRData_ItinRange

class MRData_ItinPercMove : public MRData{

public:
	mr_uint group;
	mr_uint range;
	mr_double perc_moves;


	MRData_ItinPercMove() : MRData(){
	}

	~MRData_ItinPercMove() {
	}

	int parse(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &group);
		offset += staticVarIntParse( data+offset , &range);
		perc_moves=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += staticVarIntSerialize( data+offset , group);
		offset += staticVarIntSerialize( data+offset , range);
		*( (mr_double*) (data+offset) )=perc_moves; offset +=  sizeof(mr_double);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &group);
		offset += staticVarIntParse( data+offset , &range);
		offset += sizeof(mr_double);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return group%max_num_partitions;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // comparing group
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing range
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing perc_moves
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		return 0; //If everything is equal
	}

	void copyFrom( MRData_ItinPercMove *other ){
		group=other->group;
		range=other->range;
		perc_moves=other->perc_moves;
	};
	std::string toString(){
		ostringstream o;
		o << group;

		o<<" ";
		o << range;

		o<<" ";
		o << perc_moves;

		o<<" ";
		return o.str();
	}

}; //class MRData_ItinPercMove

class MRData_Itinerary : public MRData{

public:
	mr_uint source;
	mr_uint target;
	mr_uint wday_peak_init;
	mr_uint range_peak_init;
	mr_uint wday_peak_fin;
	mr_uint range_peak_fin;
	mr_uint wday_init;
	mr_uint range_init;
	mr_uint wday_fin;
	mr_uint range_fin;


	MRData_Itinerary() : MRData(){
	}

	~MRData_Itinerary() {
	}

	int parse(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &source);
		offset += staticVarIntParse( data+offset , &target);
		offset += staticVarIntParse( data+offset , &wday_peak_init);
		offset += staticVarIntParse( data+offset , &range_peak_init);
		offset += staticVarIntParse( data+offset , &wday_peak_fin);
		offset += staticVarIntParse( data+offset , &range_peak_fin);
		offset += staticVarIntParse( data+offset , &wday_init);
		offset += staticVarIntParse( data+offset , &range_init);
		offset += staticVarIntParse( data+offset , &wday_fin);
		offset += staticVarIntParse( data+offset , &range_fin);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += staticVarIntSerialize( data+offset , source);
		offset += staticVarIntSerialize( data+offset , target);
		offset += staticVarIntSerialize( data+offset , wday_peak_init);
		offset += staticVarIntSerialize( data+offset , range_peak_init);
		offset += staticVarIntSerialize( data+offset , wday_peak_fin);
		offset += staticVarIntSerialize( data+offset , range_peak_fin);
		offset += staticVarIntSerialize( data+offset , wday_init);
		offset += staticVarIntSerialize( data+offset , range_init);
		offset += staticVarIntSerialize( data+offset , wday_fin);
		offset += staticVarIntSerialize( data+offset , range_fin);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &source);
		offset += staticVarIntParse( data+offset , &target);
		offset += staticVarIntParse( data+offset , &wday_peak_init);
		offset += staticVarIntParse( data+offset , &range_peak_init);
		offset += staticVarIntParse( data+offset , &wday_peak_fin);
		offset += staticVarIntParse( data+offset , &range_peak_fin);
		offset += staticVarIntParse( data+offset , &wday_init);
		offset += staticVarIntParse( data+offset , &range_init);
		offset += staticVarIntParse( data+offset , &wday_fin);
		offset += staticVarIntParse( data+offset , &range_fin);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return source%max_num_partitions;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // comparing source
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing target
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing wday_peak_init
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing range_peak_init
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing wday_peak_fin
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing range_peak_fin
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing wday_init
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing range_init
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing wday_fin
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing range_fin
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		return 0; //If everything is equal
	}

	void copyFrom( MRData_Itinerary *other ){
		source=other->source;
		target=other->target;
		wday_peak_init=other->wday_peak_init;
		range_peak_init=other->range_peak_init;
		wday_peak_fin=other->wday_peak_fin;
		range_peak_fin=other->range_peak_fin;
		wday_init=other->wday_init;
		range_init=other->range_init;
		wday_fin=other->wday_fin;
		range_fin=other->range_fin;
	};
	std::string toString(){
		ostringstream o;
		o << source;

		o<<" ";
		o << target;

		o<<" ";
		o << wday_peak_init;

		o<<" ";
		o << range_peak_init;

		o<<" ";
		o << wday_peak_fin;

		o<<" ";
		o << range_peak_fin;

		o<<" ";
		o << wday_init;

		o<<" ";
		o << range_init;

		o<<" ";
		o << wday_fin;

		o<<" ";
		o << range_fin;

		o<<" ";
		return o.str();
	}

}; //class MRData_Itinerary

class MRData_MatrixTime : public MRData{

public:
	MRData_Date date;
	MRData_Time time;
	mr_uint group;
	mr_uint bts;


	MRData_MatrixTime() : MRData(){
	}

	~MRData_MatrixTime() {
	}

	int parse(char *data){
		int offset=0;
		offset += date.parse(data+offset);
		offset += time.parse(data+offset);
		offset += staticVarIntParse( data+offset , &group);
		offset += staticVarIntParse( data+offset , &bts);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += date.serialize(data+offset);
		offset += time.serialize(data+offset);
		offset += staticVarIntSerialize( data+offset , group);
		offset += staticVarIntSerialize( data+offset , bts);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += date.size(data+offset);
		offset += time.size(data+offset);
		offset += staticVarIntParse( data+offset , &group);
		offset += staticVarIntParse( data+offset , &bts);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return date.getPartition(max_num_partitions);
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // comparing date
			MRData_Date _tmp_data;
			int tmp = _tmp_data.compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing time
			MRData_Time _tmp_data;
			int tmp = _tmp_data.compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing group
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing bts
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		return 0; //If everything is equal
	}

	void copyFrom( MRData_MatrixTime *other ){
		date.copyFrom(&other->date);
		time.copyFrom(&other->time);
		group=other->group;
		bts=other->bts;
	};
	std::string toString(){
		ostringstream o;
		o << date.toString();

		o<<" ";
		o << time.toString();

		o<<" ";
		o << group;

		o<<" ";
		o << bts;

		o<<" ";
		return o.str();
	}

}; //class MRData_MatrixTime

class MRData_MatrixRange : public MRData{

public:
	mr_uint node;
	mr_uint poiSrc;
	mr_uint poiTgt;
	mr_uint group;
	mr_uint range;


	MRData_MatrixRange() : MRData(){
	}

	~MRData_MatrixRange() {
	}

	int parse(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &node);
		offset += staticVarIntParse( data+offset , &poiSrc);
		offset += staticVarIntParse( data+offset , &poiTgt);
		offset += staticVarIntParse( data+offset , &group);
		offset += staticVarIntParse( data+offset , &range);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += staticVarIntSerialize( data+offset , node);
		offset += staticVarIntSerialize( data+offset , poiSrc);
		offset += staticVarIntSerialize( data+offset , poiTgt);
		offset += staticVarIntSerialize( data+offset , group);
		offset += staticVarIntSerialize( data+offset , range);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &node);
		offset += staticVarIntParse( data+offset , &poiSrc);
		offset += staticVarIntParse( data+offset , &poiTgt);
		offset += staticVarIntParse( data+offset , &group);
		offset += staticVarIntParse( data+offset , &range);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return node%max_num_partitions;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // comparing node
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing poiSrc
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing poiTgt
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing group
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing range
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		return 0; //If everything is equal
	}

	void copyFrom( MRData_MatrixRange *other ){
		node=other->node;
		poiSrc=other->poiSrc;
		poiTgt=other->poiTgt;
		group=other->group;
		range=other->range;
	};
	std::string toString(){
		ostringstream o;
		o << node;

		o<<" ";
		o << poiSrc;

		o<<" ";
		o << poiTgt;

		o<<" ";
		o << group;

		o<<" ";
		o << range;

		o<<" ";
		return o.str();
	}

}; //class MRData_MatrixRange

class MRData_PairIdMtx : public MRData{

public:
	mr_uint id1;
	mr_uint id2;
	MRData_Double *coms;
	int coms_length;
	int coms_max_length;


	MRData_PairIdMtx() : MRData(){
		coms_length=0;
		coms_max_length=0;
		coms = NULL;
	}

	~MRData_PairIdMtx() {
		if( coms )
			delete[] coms ;
	}

	int parse(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &id1);
		offset += staticVarIntParse( data+offset , &id2);
		{ //Parsing vector coms
			size_t _length;
			offset += staticVarIntParse( data+offset , &_length );
		 	comsSetLength( _length );
			for (int i = 0 ; i < (int)coms_length ; i++){
				offset += coms[i].parse(data+offset);
			}
		}
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += staticVarIntSerialize( data+offset , id1);
		offset += staticVarIntSerialize( data+offset , id2);
		{ //Serialization vector coms
			offset += staticVarIntSerialize( data+offset , coms_length );
			for (int i = 0 ; i < (int)coms_length ; i++){
				offset += coms[i].serialize(data+offset);
			}
		}
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &id1);
		offset += staticVarIntParse( data+offset , &id2);
		{ //Getting size of vector coms
			size_t _length;
			offset += staticVarIntParse( data+offset , &_length );
			MRData_Double _tmp;
			for (int i = 0 ; i < (int)_length ; i++){
				offset += _tmp.size(data+offset);
			}
		}
		return offset;
	}

	int getPartition(int max_num_partitions){
		return id1%max_num_partitions;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // comparing id1
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing id2
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // Comparing vector coms
			size_t _length1,_length2;
			*offset1 += staticVarIntParse( data1+(*offset1) , &_length1 );
			*offset2 += staticVarIntParse( data2+(*offset2) , &_length2 );
			if( _length1 < _length2 ) return -1;
			if( _length1 > _length2 ) return 1;
			for (int i = 0 ; i < (int)_length1 ; i++){
				{ // comparing coms[i]
					MRData_Double _tmp_data;
					int tmp = _tmp_data.compare(data1,data2,offset1 , offset2);
					if( tmp != 0) return tmp;
				}
			}
		}
		return 0; //If everything is equal
	}

	void comsSetLength(int _length){
		if( _length > coms_max_length){ 
			MRData_Double *_previous = coms;
			int previous_length = coms_length;
			if(coms_max_length == 0) coms_max_length = _length;
			while(coms_max_length < _length) coms_max_length *= 2;
			coms = new MRData_Double[coms_max_length ];
			if( _previous ){
				for (int i = 0 ; i < previous_length ; i++)
					coms[i].copyFrom( &_previous[i] );
				delete[] _previous;
			}
		}
		coms_length=_length;
	}

	MRData_Double* comsAdd(){
		comsSetLength( coms_length + 1 );
		return &coms[coms_length-1];
	}

	void copyFrom( MRData_PairIdMtx *other ){
		id1=other->id1;
		id2=other->id2;
			{ // CopyFrom field coms
				comsSetLength( other->coms_length);
				for (int i = 0 ; i < coms_length ; i++){
					coms[i].copyFrom(&other->coms[i]);
				}
			}
	};
	std::string toString(){
		ostringstream o;
		o << id1;

		o<<" ";
		o << id2;

		o<<" ";
		{// toString of vector coms
			for(int i = 0 ; i < coms_length ; i++){
				o << coms[i].toString();
				 o << " ";
			}
		}
		o<<" ";
		return o.str();
	}

}; //class MRData_PairIdMtx

class MRData_ProfileMtx : public MRData{

public:
	MRData_ItinRange itrange;
	mr_double count;


	MRData_ProfileMtx() : MRData(){
	}

	~MRData_ProfileMtx() {
	}

	int parse(char *data){
		int offset=0;
		offset += itrange.parse(data+offset);
		count=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += itrange.serialize(data+offset);
		*( (mr_double*) (data+offset) )=count; offset +=  sizeof(mr_double);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += itrange.size(data+offset);
		offset += sizeof(mr_double);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return itrange.getPartition(max_num_partitions);
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // comparing itrange
			MRData_ItinRange _tmp_data;
			int tmp = _tmp_data.compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing count
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		return 0; //If everything is equal
	}

	void copyFrom( MRData_ProfileMtx *other ){
		itrange.copyFrom(&other->itrange);
		count=other->count;
	};
	std::string toString(){
		ostringstream o;
		o << itrange.toString();

		o<<" ";
		o << count;

		o<<" ";
		return o.str();
	}

}; //class MRData_ProfileMtx

class MRData_NodBtsDate : public MRData{

public:
	mr_uint node;
	mr_uint bts;
	MRData_Date date;
	mr_uint hour;


	MRData_NodBtsDate() : MRData(){
	}

	~MRData_NodBtsDate() {
	}

	int parse(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &node);
		offset += staticVarIntParse( data+offset , &bts);
		offset += date.parse(data+offset);
		offset += staticVarIntParse( data+offset , &hour);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += staticVarIntSerialize( data+offset , node);
		offset += staticVarIntSerialize( data+offset , bts);
		offset += date.serialize(data+offset);
		offset += staticVarIntSerialize( data+offset , hour);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &node);
		offset += staticVarIntParse( data+offset , &bts);
		offset += date.size(data+offset);
		offset += staticVarIntParse( data+offset , &hour);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return node%max_num_partitions;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // comparing node
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing bts
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing date
			MRData_Date _tmp_data;
			int tmp = _tmp_data.compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing hour
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		return 0; //If everything is equal
	}

	void copyFrom( MRData_NodBtsDate *other ){
		node=other->node;
		bts=other->bts;
		date.copyFrom(&other->date);
		hour=other->hour;
	};
	std::string toString(){
		ostringstream o;
		o << node;

		o<<" ";
		o << bts;

		o<<" ";
		o << date.toString();

		o<<" ";
		o << hour;

		o<<" ";
		return o.str();
	}

}; //class MRData_NodBtsDate

class MRData_BtsProfile : public MRData{

public:
	mr_uint bts;
	mr_uint8 profile;
	mr_uint8 wday;
	mr_uint hour;


	MRData_BtsProfile() : MRData(){
	}

	~MRData_BtsProfile() {
	}

	int parse(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &bts);
		profile=  *( (mr_uint8*) (data+offset) ); offset +=  sizeof(mr_uint8);
		wday=  *( (mr_uint8*) (data+offset) ); offset +=  sizeof(mr_uint8);
		offset += staticVarIntParse( data+offset , &hour);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += staticVarIntSerialize( data+offset , bts);
		*( (mr_uint8*) (data+offset) )=profile; offset +=  sizeof(mr_uint8);
		*( (mr_uint8*) (data+offset) )=wday; offset +=  sizeof(mr_uint8);
		offset += staticVarIntSerialize( data+offset , hour);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &bts);
		offset += sizeof(mr_uint8);
		offset += sizeof(mr_uint8);
		offset += staticVarIntParse( data+offset , &hour);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return bts%max_num_partitions;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // comparing bts
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing profile
			if  ( *((mr_uint8*)(data1 + (*offset1))) < *((mr_uint8*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_uint8*)(data1 + (*offset1))) > *((mr_uint8*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_uint8);
			*offset2 +=sizeof(mr_uint8);
		}
		{ // comparing wday
			if  ( *((mr_uint8*)(data1 + (*offset1))) < *((mr_uint8*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_uint8*)(data1 + (*offset1))) > *((mr_uint8*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_uint8);
			*offset2 +=sizeof(mr_uint8);
		}
		{ // comparing hour
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		return 0; //If everything is equal
	}

	void copyFrom( MRData_BtsProfile *other ){
		bts=other->bts;
		profile=other->profile;
		wday=other->wday;
		hour=other->hour;
	};
	std::string toString(){
		ostringstream o;
		o << bts;

		o<<" ";
		o << (int)profile;

		o<<" ";
		o << (int)wday;

		o<<" ";
		o << hour;

		o<<" ";
		return o.str();
	}

}; //class MRData_BtsProfile

class MRData_Elipsoide : public MRData{

public:
	mr_double a;
	mr_double b;
	mr_double e;
	mr_double seg_e;
	mr_double seg_e_cua;
	mr_double c;
	mr_double apl;


	MRData_Elipsoide() : MRData(){
	}

	~MRData_Elipsoide() {
	}

	int parse(char *data){
		int offset=0;
		a=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		b=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		e=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		seg_e=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		seg_e_cua=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		c=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		apl=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		*( (mr_double*) (data+offset) )=a; offset +=  sizeof(mr_double);
		*( (mr_double*) (data+offset) )=b; offset +=  sizeof(mr_double);
		*( (mr_double*) (data+offset) )=e; offset +=  sizeof(mr_double);
		*( (mr_double*) (data+offset) )=seg_e; offset +=  sizeof(mr_double);
		*( (mr_double*) (data+offset) )=seg_e_cua; offset +=  sizeof(mr_double);
		*( (mr_double*) (data+offset) )=c; offset +=  sizeof(mr_double);
		*( (mr_double*) (data+offset) )=apl; offset +=  sizeof(mr_double);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += sizeof(mr_double);
		offset += sizeof(mr_double);
		offset += sizeof(mr_double);
		offset += sizeof(mr_double);
		offset += sizeof(mr_double);
		offset += sizeof(mr_double);
		offset += sizeof(mr_double);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return abs( (int) (a*100))%max_num_partitions;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // comparing a
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		{ // comparing b
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		{ // comparing e
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		{ // comparing seg_e
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		{ // comparing seg_e_cua
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		{ // comparing c
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		{ // comparing apl
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		return 0; //If everything is equal
	}

	void copyFrom( MRData_Elipsoide *other ){
		a=other->a;
		b=other->b;
		e=other->e;
		seg_e=other->seg_e;
		seg_e_cua=other->seg_e_cua;
		c=other->c;
		apl=other->apl;
	};
	std::string toString(){
		ostringstream o;
		o << a;

		o<<" ";
		o << b;

		o<<" ";
		o << e;

		o<<" ";
		o << seg_e;

		o<<" ";
		o << seg_e_cua;

		o<<" ";
		o << c;

		o<<" ";
		o << apl;

		o<<" ";
		return o.str();
	}

}; //class MRData_Elipsoide

class MRData_CellMxUTM : public MRData{

public:
	mr_uint cell;
	mr_uint bts;
	mr_uint mun;
	mr_uint sta;
	mr_double posx;
	mr_double posy;
	MRData_Huso huso;


	MRData_CellMxUTM() : MRData(){
	}

	~MRData_CellMxUTM() {
	}

	int parse(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &cell);
		offset += staticVarIntParse( data+offset , &bts);
		offset += staticVarIntParse( data+offset , &mun);
		offset += staticVarIntParse( data+offset , &sta);
		posx=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		posy=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		offset += huso.parse(data+offset);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += staticVarIntSerialize( data+offset , cell);
		offset += staticVarIntSerialize( data+offset , bts);
		offset += staticVarIntSerialize( data+offset , mun);
		offset += staticVarIntSerialize( data+offset , sta);
		*( (mr_double*) (data+offset) )=posx; offset +=  sizeof(mr_double);
		*( (mr_double*) (data+offset) )=posy; offset +=  sizeof(mr_double);
		offset += huso.serialize(data+offset);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &cell);
		offset += staticVarIntParse( data+offset , &bts);
		offset += staticVarIntParse( data+offset , &mun);
		offset += staticVarIntParse( data+offset , &sta);
		offset += sizeof(mr_double);
		offset += sizeof(mr_double);
		offset += huso.size(data+offset);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return cell%max_num_partitions;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // comparing cell
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing bts
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing mun
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing sta
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing posx
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		{ // comparing posy
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		{ // comparing huso
			MRData_Huso _tmp_data;
			int tmp = _tmp_data.compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		return 0; //If everything is equal
	}

	void copyFrom( MRData_CellMxUTM *other ){
		cell=other->cell;
		bts=other->bts;
		mun=other->mun;
		sta=other->sta;
		posx=other->posx;
		posy=other->posy;
		huso.copyFrom(&other->huso);
	};
	std::string toString(){
		ostringstream o;
		o << cell;

		o<<" ";
		o << bts;

		o<<" ";
		o << mun;

		o<<" ";
		o << sta;

		o<<" ";
		o << posx;

		o<<" ";
		o << posy;

		o<<" ";
		o << huso.toString();

		o<<" ";
		return o.str();
	}

}; //class MRData_CellMxUTM

class MRData_bts_coords : public MRData{

public:
	mr_uint id;
	mr_double posx;
	mr_double posy;
	MRData_Huso huso;


	MRData_bts_coords() : MRData(){
	}

	~MRData_bts_coords() {
	}

	int parse(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &id);
		posx=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		posy=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		offset += huso.parse(data+offset);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += staticVarIntSerialize( data+offset , id);
		*( (mr_double*) (data+offset) )=posx; offset +=  sizeof(mr_double);
		*( (mr_double*) (data+offset) )=posy; offset +=  sizeof(mr_double);
		offset += huso.serialize(data+offset);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &id);
		offset += sizeof(mr_double);
		offset += sizeof(mr_double);
		offset += huso.size(data+offset);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return id%max_num_partitions;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // comparing id
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing posx
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		{ // comparing posy
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		{ // comparing huso
			MRData_Huso _tmp_data;
			int tmp = _tmp_data.compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		return 0; //If everything is equal
	}

	void copyFrom( MRData_bts_coords *other ){
		id=other->id;
		posx=other->posx;
		posy=other->posy;
		huso.copyFrom(&other->huso);
	};
	std::string toString(){
		ostringstream o;
		o << id;

		o<<" ";
		o << posx;

		o<<" ";
		o << posy;

		o<<" ";
		o << huso.toString();

		o<<" ";
		return o.str();
	}

}; //class MRData_bts_coords

class MRData_Vertices_Voronoi : public MRData{

public:
	mr_uint idBts;
	mr_uint8 idOrden;
	mr_double posxUTM;
	mr_double posyUTM;
	mr_double posxGMT;
	mr_double posyGMT;
	MRData_Huso huso;


	MRData_Vertices_Voronoi() : MRData(){
	}

	~MRData_Vertices_Voronoi() {
	}

	int parse(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &idBts);
		idOrden=  *( (mr_uint8*) (data+offset) ); offset +=  sizeof(mr_uint8);
		posxUTM=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		posyUTM=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		posxGMT=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		posyGMT=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		offset += huso.parse(data+offset);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += staticVarIntSerialize( data+offset , idBts);
		*( (mr_uint8*) (data+offset) )=idOrden; offset +=  sizeof(mr_uint8);
		*( (mr_double*) (data+offset) )=posxUTM; offset +=  sizeof(mr_double);
		*( (mr_double*) (data+offset) )=posyUTM; offset +=  sizeof(mr_double);
		*( (mr_double*) (data+offset) )=posxGMT; offset +=  sizeof(mr_double);
		*( (mr_double*) (data+offset) )=posyGMT; offset +=  sizeof(mr_double);
		offset += huso.serialize(data+offset);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += staticVarIntParse( data+offset , &idBts);
		offset += sizeof(mr_uint8);
		offset += sizeof(mr_double);
		offset += sizeof(mr_double);
		offset += sizeof(mr_double);
		offset += sizeof(mr_double);
		offset += huso.size(data+offset);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return idBts%max_num_partitions;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // comparing idBts
			mr_uint _value1;
			mr_uint _value2;
			*offset1 += staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing idOrden
			if  ( *((mr_uint8*)(data1 + (*offset1))) < *((mr_uint8*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_uint8*)(data1 + (*offset1))) > *((mr_uint8*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_uint8);
			*offset2 +=sizeof(mr_uint8);
		}
		{ // comparing posxUTM
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		{ // comparing posyUTM
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		{ // comparing posxGMT
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		{ // comparing posyGMT
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		{ // comparing huso
			MRData_Huso _tmp_data;
			int tmp = _tmp_data.compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		return 0; //If everything is equal
	}

	void copyFrom( MRData_Vertices_Voronoi *other ){
		idBts=other->idBts;
		idOrden=other->idOrden;
		posxUTM=other->posxUTM;
		posyUTM=other->posyUTM;
		posxGMT=other->posxGMT;
		posyGMT=other->posyGMT;
		huso.copyFrom(&other->huso);
	};
	std::string toString(){
		ostringstream o;
		o << idBts;

		o<<" ";
		o << (int)idOrden;

		o<<" ";
		o << posxUTM;

		o<<" ";
		o << posyUTM;

		o<<" ";
		o << posxGMT;

		o<<" ";
		o << posyGMT;

		o<<" ";
		o << huso.toString();

		o<<" ";
		return o.str();
	}

}; //class MRData_Vertices_Voronoi

class MRData_Contorno : public MRData{

public:
	mr_double posx;
	mr_double posy;


	MRData_Contorno() : MRData(){
	}

	~MRData_Contorno() {
	}

	int parse(char *data){
		int offset=0;
		posx=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		posy=  *( (mr_double*) (data+offset) ); offset +=  sizeof(mr_double);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		*( (mr_double*) (data+offset) )=posx; offset +=  sizeof(mr_double);
		*( (mr_double*) (data+offset) )=posy; offset +=  sizeof(mr_double);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += sizeof(mr_double);
		offset += sizeof(mr_double);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return abs( (int) (posx*100))%max_num_partitions;
	}

	int compare(char * data1 , char *data2 , int *offset1 , int *offset2 ){
		{ // comparing posx
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		{ // comparing posy
			if  ( *((mr_double*)(data1 + (*offset1))) < *((mr_double*)(data2 + (*offset2))) ) return -1;
			if  ( *((mr_double*)(data1 + (*offset1))) > *((mr_double*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(mr_double);
			*offset2 +=sizeof(mr_double);
		}
		return 0; //If everything is equal
	}

	void copyFrom( MRData_Contorno *other ){
		posx=other->posx;
		posy=other->posy;
	};
	std::string toString(){
		ostringstream o;
		o << posx;

		o<<" ";
		o << posy;

		o<<" ";
		return o.str();
	}

}; //class MRData_Contorno


// Operations defined in this module


class MACRO_mob_script_parse_data : MACRO_Script {

	public:


	//Constructor
	MACRO_mob_script_parse_data() : MACRO_Script( KVFormat::format( "txt" , "txt" ) , KVFormat::format( "Void" , "Void" ) )
	{
		top = true;
	}

	//Main function to implement

	std::string help(){
		ostringstream o;
		o<<"Parse text data to binary data\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
};

class MACRO_mob_script_prepare_data : MACRO_Script {

	public:


	//Constructor
	MACRO_mob_script_prepare_data() : MACRO_Script( KVFormat::format( "txt" , "txt" ) , KVFormat::format( "Void" , "Void" ) )
	{
		top = true;
	}

	//Main function to implement

	std::string help(){
		ostringstream o;
		o<<"Prepare binary data to the mobility process\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
};

class MACRO_mob_script_get_vectors_to_cluster : MACRO_Script {

	public:


	//Constructor
	MACRO_mob_script_get_vectors_to_cluster() : MACRO_Script( KVFormat::format( "txt" , "txt" ) , KVFormat::format( "Void" , "Void" ) )
	{
		top = true;
	}

	//Main function to implement

	std::string help(){
		ostringstream o;
		o<<"Calculate vectors of communication (client, bts, poi)\n";
		o<<"inputs of cluster process\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
};

class MACRO_mob_script_extract_MIVs : MACRO_Script {

	public:


	//Constructor
	MACRO_mob_script_extract_MIVs() : MACRO_Script( KVFormat::format( "txt" , "txt" ) , KVFormat::format( "Void" , "Void" ) )
	{
		top = true;
	}

	//Main function to implement

	std::string help(){
		ostringstream o;
		o<<"Script to calculate the mobility individual variables of a client.\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
};

class MACRO_mob_script_extract_POIs : MACRO_Script {

	public:


	//Constructor
	MACRO_mob_script_extract_POIs() : MACRO_Script( KVFormat::format( "txt" , "txt" ) , KVFormat::format( "Void" , "Void" ) )
	{
		top = true;
	}

	//Main function to implement

	std::string help(){
		ostringstream o;
		o<<"Script to identify the points of interest of a client.\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
};

class MACRO_mob_script_label_POIs : MACRO_Script {

	public:


	//Constructor
	MACRO_mob_script_label_POIs() : MACRO_Script( KVFormat::format( "txt" , "txt" ) , KVFormat::format( "Void" , "Void" ) )
	{
		top = true;
	}

	//Main function to implement

	std::string help(){
		ostringstream o;
		o<<"Script to label the points of interest.\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
};

class MACRO_mob_script_label_client : MACRO_Script {

	public:


	//Constructor
	MACRO_mob_script_label_client() : MACRO_Script( KVFormat::format( "txt" , "txt" ) , KVFormat::format( "Void" , "Void" ) )
	{
		top = true;
	}

	//Main function to implement

	std::string help(){
		ostringstream o;
		o<<"Script to label the general use of a client.\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
};

class MACRO_mob_script_label_bts : MACRO_Script {

	public:


	//Constructor
	MACRO_mob_script_label_bts() : MACRO_Script( KVFormat::format( "txt" , "txt" ) , KVFormat::format( "Void" , "Void" ) )
	{
		top = true;
	}

	//Main function to implement

	std::string help(){
		ostringstream o;
		o<<"Script to label the general use of a bts.\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
};

class MACRO_mob_script_label_client_bts : MACRO_Script {

	public:


	//Constructor
	MACRO_mob_script_label_client_bts() : MACRO_Script( KVFormat::format( "txt" , "txt" ) , KVFormat::format( "Void" , "Void" ) )
	{
		top = true;
	}

	//Main function to implement

	std::string help(){
		ostringstream o;
		o<<"Script to label the use of a bts by a client.\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
};

class MACRO_mob_script_join_labels : MACRO_Script {

	public:


	//Constructor
	MACRO_mob_script_join_labels() : MACRO_Script( KVFormat::format( "txt" , "txt" ) , KVFormat::format( "Void" , "Void" ) )
	{
		top = true;
	}

	//Main function to implement

	std::string help(){
		ostringstream o;
		o<<"Script to join labels of points of interest of a client.\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
};

class MACRO_mob_script_detect_second_homes : MACRO_Script {

	public:


	//Constructor
	MACRO_mob_script_detect_second_homes() : MACRO_Script( KVFormat::format( "txt" , "txt" ) , KVFormat::format( "Void" , "Void" ) )
	{
		top = true;
	}

	//Main function to implement

	std::string help(){
		ostringstream o;
		o<<"Script to detect second homes.\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
};

class MACRO_mob_script_out_pois : MACRO_Script {

	public:


	//Constructor
	MACRO_mob_script_out_pois() : MACRO_Script( KVFormat::format( "txt" , "txt" ) , KVFormat::format( "Void" , "Void" ) )
	{
		top = true;
	}

	//Main function to implement

	std::string help(){
		ostringstream o;
		o<<"Script to detect second homes.\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
};

class MACRO_mob_script_get_itineraries : MACRO_Script {

	public:


	//Constructor
	MACRO_mob_script_get_itineraries() : MACRO_Script( KVFormat::format( "txt" , "txt" ) , KVFormat::format( "Void" , "Void" ) )
	{
		top = true;
	}

	//Main function to implement

	std::string help(){
		ostringstream o;
		o<<"Script to extract itineraries between pois of clients.\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
};

class MACRO_mob_script_get_population_density : MACRO_Script {

	public:


	//Constructor
	MACRO_mob_script_get_population_density() : MACRO_Script( KVFormat::format( "txt" , "txt" ) , KVFormat::format( "Void" , "Void" ) )
	{
		top = true;
	}

	//Main function to implement

	std::string help(){
		ostringstream o;
		o<<"Script to extract density of population matrixes\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
};

class MACRO_mob_script_get_population_density_profile : MACRO_Script {

	public:


	//Constructor
	MACRO_mob_script_get_population_density_profile() : MACRO_Script( KVFormat::format( "txt" , "txt" ) , KVFormat::format( "Void" , "Void" ) )
	{
		top = true;
	}

	//Main function to implement

	std::string help(){
		ostringstream o;
		o<<"Script to extract density of population matrixes by client profile\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
};

class MACRO_mobmx_parse_cdrs : MRParser {

	public:


	//Constructor
	MACRO_mobmx_parse_cdrs() : MRParser( KVFormat::format( "UInt" , "CdrMx" ) )
	{
	}

	//Main function to implement
	void parseLines( std::vector<char*> lines , bool eof, KVWriterInterface *writer );

	std::string help(){
		ostringstream o;
		o<<"Parse a txt file to extract TEMM cdrs\n";
		o<<"Output 1: cdrs with cell info\n";
		o<<"Output 2: cdrs without cell info\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_parse_cdrs::parseLines( std::vector<char*> lines , bool eof, KVWriterInterface *writer ){}
};

class MACRO_mobmx_get_sample_10000 : MACRO_Map {

	public:


	//Constructor
	MACRO_mobmx_get_sample_10000() : MACRO_Map( KVFormat::format( "UInt" , "CdrMx" ) , KVFormat::format( "UInt" , "CdrMx" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Extract a sample of around 10000 clients\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_get_sample_10000::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_parse_cells : MRParser {

	public:


	//Constructor
	MACRO_mobmx_parse_cells() : MRParser( KVFormat::format( "UInt" , "CellMx" ) )
	{
	}

	//Main function to implement
	void parseLines( std::vector<char*> lines , bool eof, KVWriterInterface *writer );

	std::string help(){
		ostringstream o;
		o<<"Parse a txt file to extract TEMM cells\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_parse_cells::parseLines( std::vector<char*> lines , bool eof, KVWriterInterface *writer ){}
};

class MACRO_mobmx_parse_contorno : MRParser {

	public:


	//Constructor
	MACRO_mobmx_parse_contorno() : MRParser( KVFormat::format( "UInt" , "Contorno" ) )
	{
	}

	//Main function to implement
	void parseLines( std::vector<char*> lines , bool eof, KVWriterInterface *writer );

	std::string help(){
		ostringstream o;
		o<<"Parse a txt file to extract the contour coords of a country in order to apply Voronoi\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_parse_contorno::parseLines( std::vector<char*> lines , bool eof, KVWriterInterface *writer ){}
};

class MACRO_mobmx_parse_client_clusters : MRParser {

	public:


	//Constructor
	MACRO_mobmx_parse_client_clusters() : MRParser( KVFormat::format( "UInt" , "Cluster" ) )
	{
	}

	//Main function to implement
	void parseLines( std::vector<char*> lines , bool eof, KVWriterInterface *writer );

	std::string help(){
		ostringstream o;
		o<<"Parse a txt file to extract client clusters\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_parse_client_clusters::parseLines( std::vector<char*> lines , bool eof, KVWriterInterface *writer ){}
};

class MACRO_mobmx_parse_clientbts_clusters : MRParser {

	public:


	//Constructor
	MACRO_mobmx_parse_clientbts_clusters() : MRParser( KVFormat::format( "UInt" , "Cluster" ) )
	{
	}

	//Main function to implement
	void parseLines( std::vector<char*> lines , bool eof, KVWriterInterface *writer );

	std::string help(){
		ostringstream o;
		o<<"Parse a txt file to extract clientbts clusters\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_parse_clientbts_clusters::parseLines( std::vector<char*> lines , bool eof, KVWriterInterface *writer ){}
};

class MACRO_mobmx_parse_bts_clusters : MRParser {

	public:


	//Constructor
	MACRO_mobmx_parse_bts_clusters() : MRParser( KVFormat::format( "UInt" , "Cluster" ) )
	{
	}

	//Main function to implement
	void parseLines( std::vector<char*> lines , bool eof, KVWriterInterface *writer );

	std::string help(){
		ostringstream o;
		o<<"Parse a txt file to extract bts clusters\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_parse_bts_clusters::parseLines( std::vector<char*> lines , bool eof, KVWriterInterface *writer ){}
};

class MACRO_mobmx_get_mob_from_cdrs : MACRO_Map {

	public:


	//Constructor
	MACRO_mobmx_get_mob_from_cdrs() : MACRO_Map( KVFormat::format( "UInt" , "GSTCdr" ) , KVFormat::format( "UInt" , "CdrMx" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Extract mobility info from gasset cdrs\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_get_mob_from_cdrs::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_get_mob_from_cells : MACRO_Map {

	public:


	//Constructor
	MACRO_mobmx_get_mob_from_cells() : MACRO_Map( KVFormat::format( "UInt" , "GSTCell" ) , KVFormat::format( "UInt" , "CellMx" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Extract mobility info from gasset cells\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_get_mob_from_cells::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_filter_cellnoinfo : MACRO_Map {

	public:


	//Constructor
	MACRO_mobmx_filter_cellnoinfo() : MACRO_Map( KVFormat::format( "UInt" , "CdrMx" ) , KVFormat::vectorOfFormats(KVFormat::format( "UInt" , "CdrMx" ),KVFormat::format( "UInt" , "CdrMx" )) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Spread cdrs by cell, and filt them by cellinfo/cellnoinfo\n";
		o<<"Output 1: cdrs with cell info\n";
		o<<"Output 2: cdrs without cell info\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_filter_cellnoinfo::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_join_bts_node : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_join_bts_node() : MACRO_Reduce( KVFormat::vectorOfFormats(KVFormat::format( "UInt" , "CdrMx" ),KVFormat::format( "UInt" , "CellMx" )) , KVFormat::vectorOfFormats(KVFormat::format( "Node_Bts" , "Void" ),KVFormat::format( "UInt" , "TwoInt" ),KVFormat::format( "UInt" , "CdrMx" ),KVFormat::format( "TelMonth" , "CellMx" )) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Change the cell id by bts id\n";
		o<<"Input 1: cdrs\n";
		o<<"Input 2: cell catalogue\n";
		o<<"Output 1: node, bts, wday and range - void\n";
		o<<"Output 2: bts - wday and range\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_join_bts_node::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_node_bts_counter : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_node_bts_counter() : MACRO_Reduce( KVFormat::format( "Node_Bts" , "Void" ) , KVFormat::format( "UInt" , "Bts_Counter" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Count number of comms by node, bts, day and hour\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_node_bts_counter::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_node_mobInfo : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_node_mobInfo() : MACRO_Reduce( KVFormat::format( "UInt" , "Bts_Counter" ) , KVFormat::format( "UInt" , "NodeMx_Counter" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Create the array of counters by node: num of comms by day and hour\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_node_mobInfo::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_repbts_spread_nodebts : MACRO_Map {

	public:


	//Constructor
	MACRO_mobmx_repbts_spread_nodebts() : MACRO_Map( KVFormat::format( "UInt" , "NodeMx_Counter" ) , KVFormat::format( "Node_Bts_Day" , "UInt" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Spread client counters by node and bts\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_repbts_spread_nodebts::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_repbts_aggbybts : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_repbts_aggbybts() : MACRO_Reduce( KVFormat::format( "Node_Bts_Day" , "UInt" ) , KVFormat::format( "UInt" , "Node_Bts_Day" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Add the number comms of a client in a bts\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_repbts_aggbybts::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_repbts_filter_num_comms : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_repbts_filter_num_comms() : MACRO_Reduce( KVFormat::vectorOfFormats(KVFormat::format( "UInt" , "Node_Bts_Day" ),KVFormat::format( "UInt" , "CdrMx" ),KVFormat::format( "UInt" , "CdrMx" )) , KVFormat::format( "UInt" , "UInt" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Count of number of coms by client, and filter according the total number communications\n";
		o<<"Output1: Clients in filter\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_repbts_filter_num_comms::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_repbts_join_dist_comms : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_repbts_join_dist_comms() : MACRO_Reduce( KVFormat::vectorOfFormats(KVFormat::format( "UInt" , "Node_Bts_Day" ),KVFormat::format( "UInt" , "UInt" )) , KVFormat::format( "UInt" , "Bts_Counter" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Get percentages of communication of a client by bts and range\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_repbts_join_dist_comms::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_repbts_get_representative_bts : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_repbts_get_representative_bts() : MACRO_Reduce( KVFormat::format( "UInt" , "Bts_Counter" ) , KVFormat::format( "TwoInt" , "Bts_Counter" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Extract the most representative btss for a node\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_repbts_get_representative_bts::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_vector_normalized : MACRO_Map {

	public:


	//Constructor
	MACRO_mobmx_vector_normalized() : MACRO_Map( KVFormat::format( "Node_Bts" , "ClusterVector" ) , KVFormat::format( "Node_Bts" , "ClusterVector" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Normalization of a vector\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_vector_normalized::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_vector_filt_clients : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_vector_filt_clients() : MACRO_Reduce( KVFormat::vectorOfFormats(KVFormat::format( "UInt" , "CdrMx" ),KVFormat::format( "UInt" , "UInt" )) , KVFormat::format( "UInt" , "CdrMx" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Filter cdrs to clients filtered by total comms\n";
		o<<"Input 1: Cdrs\n";
		o<<"Input 2: List of clients filtered by total comms\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_vector_filt_clients::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_vector_spread_nodedayhour : MACRO_Map {

	public:


	//Constructor
	MACRO_mobmx_vector_spread_nodedayhour() : MACRO_Map( KVFormat::format( "UInt" , "CdrMx" ) , KVFormat::format( "Node_Bts" , "Void" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Spread cdrs by node, day and hour\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_vector_spread_nodedayhour::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_vector_get_ncoms_nodedayhour : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_vector_get_ncoms_nodedayhour() : MACRO_Reduce( KVFormat::format( "Node_Bts" , "Void" ) , KVFormat::format( "Node_Bts" , "TwoInt" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Get number of comms by node, day and hour\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_vector_get_ncoms_nodedayhour::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_vector_create_node_dayhour : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_vector_create_node_dayhour() : MACRO_Reduce( KVFormat::format( "Node_Bts" , "TwoInt" ) , KVFormat::format( "TwoInt" , "DailyVector" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Create the vector of node comms by day and hour\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_vector_create_node_dayhour::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_vector_fuse_node_daygroup : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_vector_fuse_node_daygroup() : MACRO_Reduce( KVFormat::format( "TwoInt" , "DailyVector" ) , KVFormat::format( "Node_Bts" , "ClusterVector" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Fuse the vector of node comms by group day\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_vector_fuse_node_daygroup::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_vector_spread_nodbts : MACRO_Map {

	public:


	//Constructor
	MACRO_mobmx_vector_spread_nodbts() : MACRO_Map( KVFormat::format( "UInt" , "NodeMx_Counter" ) , KVFormat::format( "Node_Bts" , "UInt" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Spread clients info array by node, bts, group and hour.\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_vector_spread_nodbts::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_vector_sum_groupcomms : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_vector_sum_groupcomms() : MACRO_Reduce( KVFormat::format( "Node_Bts" , "UInt" ) , KVFormat::format( "TwoInt" , "Bts_Counter" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Add num of comms of a client in a bts by group and hour\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_vector_sum_groupcomms::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_vector_filt_clientbts : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_vector_filt_clientbts() : MACRO_Reduce( KVFormat::vectorOfFormats(KVFormat::format( "TwoInt" , "Bts_Counter" ),KVFormat::format( "TwoInt" , "Bts_Counter" )) , KVFormat::format( "Node_Bts" , "TwoInt" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Filter pair clients-bts to representative btss of a client\n";
		o<<"Input 1: Node-bts sum of group comms by hour\n";
		o<<"Input 2: Representative bts\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_vector_filt_clientbts::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_vector_filter_bts : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_vector_filter_bts() : MACRO_Reduce( KVFormat::format( "UInt" , "TwoInt" ) , KVFormat::format( "Bts_Counter" , "Void" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Filter bts by num of comms total and max area\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_vector_filter_bts::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_vector_sum_coms_bts : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_vector_sum_coms_bts() : MACRO_Reduce( KVFormat::format( "Bts_Counter" , "Void" ) , KVFormat::format( "Node_Bts" , "TwoInt" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Sum number of comms by bts, group and hour\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_vector_sum_coms_bts::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_cluster_client_get_min_distance : MACRO_Map {

	public:


	//Constructor
	MACRO_mobmx_cluster_client_get_min_distance() : MACRO_Map( KVFormat::format( "Node_Bts" , "ClusterVector" ) , KVFormat::format( "UInt" , "Cluster" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Calculate the euclidean distance between a vector and a cluster\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_cluster_client_get_min_distance::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_cluster_clientbts_get_min_distance : MACRO_Map {

	public:


	//Constructor
	MACRO_mobmx_cluster_clientbts_get_min_distance() : MACRO_Map( KVFormat::format( "Node_Bts" , "ClusterVector" ) , KVFormat::vectorOfFormats(KVFormat::format( "UInt" , "Poi" ),KVFormat::format( "TwoInt" , "Cluster" )) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Calculate the pearson distance between a vector and a cluster\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_cluster_clientbts_get_min_distance::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_cluster_bts_get_min_distance : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_cluster_bts_get_min_distance() : MACRO_Reduce( KVFormat::format( "Node_Bts" , "ClusterVector" ) , KVFormat::format( "UInt" , "Cluster" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Calculate the euclidean distance between a vector and a cluster.\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_cluster_bts_get_min_distance::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_filter_bts_vector : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_filter_bts_vector() : MACRO_Reduce( KVFormat::vectorOfFormats(KVFormat::format( "UInt" , "Cluster" ),KVFormat::format( "UInt" , "Bts" )) , KVFormat::format( "UInt" , "Cluster" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Filter bts vector by num of comms and area\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_filter_bts_vector::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_cluster_agg_node_cluster : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_cluster_agg_node_cluster() : MACRO_Reduce( KVFormat::vectorOfFormats(KVFormat::format( "UInt" , "Poi" ),KVFormat::format( "UInt" , "Cluster" )) , KVFormat::vectorOfFormats(KVFormat::format( "TwoInt" , "Poi" ),KVFormat::format( "UInt" , "Poi" )) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Aggregate to results info about client cluster\n";
		o<<"Input 1: POI with clientbts cluster info\n";
		o<<"Input 2: Info about client cluster\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_cluster_agg_node_cluster::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_cluster_spread_nodelbl_poilbl : MACRO_Map {

	public:


	//Constructor
	MACRO_mobmx_cluster_spread_nodelbl_poilbl() : MACRO_Map( KVFormat::format( "TwoInt" , "Poi" ) , KVFormat::format( "TwoInt" , "Void" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Spread points of interest by client label and poi label\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_cluster_spread_nodelbl_poilbl::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_cluster_count_majPoi_by_node : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_cluster_count_majPoi_by_node() : MACRO_Reduce( KVFormat::format( "TwoInt" , "Void" ) , KVFormat::format( "UInt" , "TwoInt" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Count number of times pair node label - poi label\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_cluster_count_majPoi_by_node::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_cluster_get_majPoi_by_node : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_cluster_get_majPoi_by_node() : MACRO_Reduce( KVFormat::format( "UInt" , "TwoInt" ) , KVFormat::format( "UInt" , "UInt" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Get max majority POI label by node label\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_cluster_get_majPoi_by_node::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_cluster_join_potPoi_label : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_cluster_join_potPoi_label() : MACRO_Reduce( KVFormat::vectorOfFormats(KVFormat::format( "UInt" , "Poi" ),KVFormat::format( "UInt" , "UInt" )) , KVFormat::format( "TwoInt" , "Void" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Set label to pois by majority node label\n";
		o<<"Input 1: potential pois\n";
		o<<"Input 2: node label - majority poi label\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_cluster_join_potPoi_label::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_cluster_agg_potPoi_pois : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_cluster_agg_potPoi_pois() : MACRO_Reduce( KVFormat::vectorOfFormats(KVFormat::format( "TwoInt" , "Poi" ),KVFormat::format( "TwoInt" , "Cluster" ),KVFormat::format( "TwoInt" , "Void" )) , KVFormat::vectorOfFormats(KVFormat::format( "UInt" , "Poi" ),KVFormat::format( "TwoInt" , "Cluster" )) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Aggregate the potential pois labeled to all pois\n";
		o<<"Input 1: all pois\n";
		o<<"Input 2: client communications vector\n";
		o<<"Input 3: potential pois\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_cluster_agg_potPoi_pois::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_cluster_agg_bts_cluster : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_cluster_agg_bts_cluster() : MACRO_Reduce( KVFormat::vectorOfFormats(KVFormat::format( "UInt" , "Poi" ),KVFormat::format( "UInt" , "Cluster" )) , KVFormat::format( "UInt" , "Poi" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Aggregate to results info about bts cluster\n";
		o<<"Input 1: POI with clientbts cluster info and client cluster info\n";
		o<<"Input 2: Info about bts cluster\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_cluster_agg_bts_cluster::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_poi_cell_to_bts : MACRO_Map {

	public:


	//Constructor
	MACRO_mobmx_poi_cell_to_bts() : MACRO_Map( KVFormat::format( "UInt" , "CellMx" ) , KVFormat::format( "UInt" , "CellMx" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Convert the cell catalogue to bts catalogue\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_poi_cell_to_bts::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_poi_join_pois_btscoord : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_poi_join_pois_btscoord() : MACRO_Reduce( KVFormat::vectorOfFormats(KVFormat::format( "UInt" , "Poi" ),KVFormat::format( "UInt" , "CellMx" )) , KVFormat::vectorOfFormats(KVFormat::format( "UInt" , "PoiPos" ),KVFormat::format( "TwoInt" , "Poi" )) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Add bts coords to the list of pois\n";
		o<<"Input 1: list of pois\n";
		o<<"Input 2: bts catalogue\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_poi_join_pois_btscoord::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_poi_join_pois_vi : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_poi_join_pois_vi() : MACRO_Reduce( KVFormat::vectorOfFormats(KVFormat::format( "UInt" , "PoiPos" ),KVFormat::format( "UInt" , "Mob_VI_MobVars" )) , KVFormat::vectorOfFormats(KVFormat::format( "UInt" , "PoiPos" ),KVFormat::format( "TwoInt" , "TwoInt" )) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Join points of interest (client-bts) and individual variables\n";
		o<<"to determine if a poi is in or out the radius of influence area\n";
		o<<"Input 1: List of pois (client-bts)\n";
		o<<"Input 2: List of individual variables\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_poi_join_pois_vi::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_poi_get_pairs_sechome_pois : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_poi_get_pairs_sechome_pois() : MACRO_Reduce( KVFormat::format( "UInt" , "PoiPos" ) , KVFormat::format( "TwoInt" , "UInt" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Extract pair of home bts, first in Monday-Friday radius, and the second one out\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_poi_get_pairs_sechome_pois::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_poi_filter_sechome_adjacent : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_poi_filter_sechome_adjacent() : MACRO_Reduce( KVFormat::vectorOfFormats(KVFormat::format( "TwoInt" , "UInt" ),KVFormat::format( "TwoInt" , "Void" )) , KVFormat::format( "TwoInt" , "Void" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Filter potential secondary homes that are adjacents to first home\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_poi_filter_sechome_adjacent::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_poi_delete_sechome_duplicate : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_poi_delete_sechome_duplicate() : MACRO_Reduce( KVFormat::format( "TwoInt" , "Void" ) , KVFormat::format( "TwoInt" , "Void" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Delete secondary homes duplicated\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_poi_delete_sechome_duplicate::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_poi_join_sechome_results : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_poi_join_sechome_results() : MACRO_Reduce( KVFormat::vectorOfFormats(KVFormat::format( "TwoInt" , "Poi" ),KVFormat::format( "TwoInt" , "TwoInt" ),KVFormat::format( "TwoInt" , "Void" )) , KVFormat::format( "TwoInt" , "Poi" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Aggregate the secondary home results\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_poi_join_sechome_results::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_adj_parse_adj_bts : MRParser {

	public:


	//Constructor
	MACRO_mobmx_adj_parse_adj_bts() : MRParser( KVFormat::format( "TwoInt" , "Void" ) )
	{
	}

	//Main function to implement
	void parseLines( std::vector<char*> lines , bool eof, KVWriterInterface *writer );

	std::string help(){
		ostringstream o;
		o<<"Parse a txt file to extract pair of adjacent bts\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_adj_parse_adj_bts::parseLines( std::vector<char*> lines , bool eof, KVWriterInterface *writer ){}
};

class MACRO_mobmx_adj_add_unique_id_poi : MACRO_Map {

	public:


	//Constructor
	MACRO_mobmx_adj_add_unique_id_poi() : MACRO_Map( KVFormat::format( "TwoInt" , "Poi" ) , KVFormat::vectorOfFormats(KVFormat::format( "TwoInt" , "PoiNew" ),KVFormat::format( "UInt" , "TwoInt" )) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Add unique id to POIs\n";
		o<<"Output 1: List of POIs with id\n";
		o<<"Output 2: Table of POIs id\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_adj_add_unique_id_poi::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_adj_group_type_poi_client : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_adj_group_type_poi_client() : MACRO_Reduce( KVFormat::format( "TwoInt" , "PoiNew" ) , KVFormat::format( "TwoInt" , "TwoInt" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Get pair of bts of a node with the same poi label\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_adj_group_type_poi_client::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_adj_join_pairbts_adjbts : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_adj_join_pairbts_adjbts() : MACRO_Reduce( KVFormat::vectorOfFormats(KVFormat::format( "TwoInt" , "TwoInt" ),KVFormat::format( "TwoInt" , "Void" )) , KVFormat::format( "UInt" , "TwoInt" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Join pair of bts with same POI of a client to pair of adjacent bts\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_adj_join_pairbts_adjbts::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_adj_spread_count : MACRO_Map {

	public:


	//Constructor
	MACRO_mobmx_adj_spread_count() : MACRO_Map( KVFormat::format( "UInt" , "TwoInt" ) , KVFormat::format( "UInt" , "UInt" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Spread pair of pois by 1\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_adj_spread_count::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_adj_count_indexes : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_adj_count_indexes() : MACRO_Reduce( KVFormat::format( "UInt" , "UInt" ) , KVFormat::format( "UInt" , "Void" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Count number of pair of pois\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_adj_count_indexes::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_adj_put_max_id : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_adj_put_max_id() : MACRO_Reduce( KVFormat::format( "UInt" , "TwoInt" ) , KVFormat::format( "UInt" , "TwoInt" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Put the max poi id to all adjacent pois respect to poi1\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_adj_put_max_id::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_adj_update_pois_table : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_adj_update_pois_table() : MACRO_Reduce( KVFormat::vectorOfFormats(KVFormat::format( "UInt" , "TwoInt" ),KVFormat::format( "UInt" , "TwoInt" )) , KVFormat::format( "UInt" , "TwoInt" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Update the poiId with the adjacent\n";
		o<<"Input 1: Pois id table\n";
		o<<"Input 2: List of indexes\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_adj_update_pois_table::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_adj_swap_poiId_st1 : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_adj_swap_poiId_st1() : MACRO_Reduce( KVFormat::vectorOfFormats(KVFormat::format( "UInt" , "TwoInt" ),KVFormat::format( "UInt" , "TwoInt" )) , KVFormat::format( "UInt" , "TwoInt" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Change the poi1 by the respective poi in the index\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_adj_swap_poiId_st1::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_adj_swap_poiId_st2 : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_adj_swap_poiId_st2() : MACRO_Reduce( KVFormat::vectorOfFormats(KVFormat::format( "UInt" , "TwoInt" ),KVFormat::format( "UInt" , "TwoInt" )) , KVFormat::format( "UInt" , "TwoInt" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Change the poi2 by the respective poi in the index\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_adj_swap_poiId_st2::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_adj_spread_table_by_poiId : MACRO_Map {

	public:


	//Constructor
	MACRO_mobmx_adj_spread_table_by_poiId() : MACRO_Map( KVFormat::format( "UInt" , "TwoInt" ) , KVFormat::format( "UInt" , "UInt" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Spread table of pois and modified pois by pois id\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_adj_spread_table_by_poiId::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_adj_spread_pois_by_poiId : MACRO_Map {

	public:


	//Constructor
	MACRO_mobmx_adj_spread_pois_by_poiId() : MACRO_Map( KVFormat::format( "TwoInt" , "PoiNew" ) , KVFormat::format( "UInt" , "PoiNew" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Spread list of pois by pois id\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_adj_spread_pois_by_poiId::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_adj_join_new_poiId : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_adj_join_new_poiId() : MACRO_Reduce( KVFormat::vectorOfFormats(KVFormat::format( "UInt" , "UInt" ),KVFormat::format( "UInt" , "PoiNew" )) , KVFormat::format( "TwoInt" , "PoiNew" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Change in the list of pois the new poiId\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_adj_join_new_poiId::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_adj_change_poisId : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_adj_change_poisId() : MACRO_Reduce( KVFormat::vectorOfFormats(KVFormat::format( "TwoInt" , "Poi" ),KVFormat::format( "TwoInt" , "PoiNew" )) , KVFormat::format( "TwoInt" , "Poi" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Change id of pois\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_adj_change_poisId::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_poi_spread_nodebts_vector : MACRO_Map {

	public:


	//Constructor
	MACRO_mobmx_poi_spread_nodebts_vector() : MACRO_Map( KVFormat::format( "Node_Bts" , "ClusterVector" ) , KVFormat::format( "TwoInt" , "ClusterVector" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Spread node-bts vector by node-bts\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_poi_spread_nodebts_vector::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_poi_join_poivector_poi : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_poi_join_poivector_poi() : MACRO_Reduce( KVFormat::vectorOfFormats(KVFormat::format( "TwoInt" , "ClusterVector" ),KVFormat::format( "TwoInt" , "Poi" )) , KVFormat::format( "TwoInt" , "Cluster" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Join poi vector to poi info\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_poi_join_poivector_poi::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_poi_normalize_poi_vector : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_poi_normalize_poi_vector() : MACRO_Reduce( KVFormat::format( "TwoInt" , "Cluster" ) , KVFormat::format( "TwoInt" , "Cluster" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Normalize poi vector\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_poi_normalize_poi_vector::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_vector_oneid_out : MRParserOut {

	public:
	char * sep ;


	//Constructor
	MACRO_mobmx_vector_oneid_out() : MRParserOut( KVFormat::format( "UInt" , "Cluster" ) )
	{
	}

	//Main function to implement
	int parseOut( KVSet vector , FILE *file);
	int parseOutFinish( FILE *file);

	std::string help(){
		ostringstream o;
		o<<"Parse out of a vector\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//int MACRO_mobmx_vector_oneid_out::parseOut( KVSet kvSet , FILE *file){}
	//int MACRO_mobmx_vector_oneid_out::parseOutFinish( FILE *file){}
};

class MACRO_mobmx_vector_nodbts_out : MRParserOut {

	public:
	char * sep ;


	//Constructor
	MACRO_mobmx_vector_nodbts_out() : MRParserOut( KVFormat::format( "TwoInt" , "Cluster" ) )
	{
	}

	//Main function to implement
	int parseOut( KVSet vector , FILE *file);
	int parseOutFinish( FILE *file);

	std::string help(){
		ostringstream o;
		o<<"Parse out of a vector\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//int MACRO_mobmx_vector_nodbts_out::parseOut( KVSet kvSet , FILE *file){}
	//int MACRO_mobmx_vector_nodbts_out::parseOutFinish( FILE *file){}
};

class MACRO_mobmx_vector_pois_out : MRParserOut {

	public:
	char * sep ;


	//Constructor
	MACRO_mobmx_vector_pois_out() : MRParserOut( KVFormat::format( "TwoInt" , "Poi" ) )
	{
	}

	//Main function to implement
	int parseOut( KVSet vector , FILE *file);
	int parseOutFinish( FILE *file);

	std::string help(){
		ostringstream o;
		o<<"Parse out of points of interest\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//int MACRO_mobmx_vector_pois_out::parseOut( KVSet kvSet , FILE *file){}
	//int MACRO_mobmx_vector_pois_out::parseOutFinish( FILE *file){}
};

class MACRO_mobmx_MobVar_calcvariables : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_MobVar_calcvariables() : MACRO_Reduce( KVFormat::format( "TelMonth" , "CellMx" ) , KVFormat::format( "UInt" , "MobVars" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Calculate individual variables by period\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_MobVar_calcvariables::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_MobVar_fusiontotalvars : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_MobVar_fusiontotalvars() : MACRO_Reduce( KVFormat::format( "UInt" , "MobVars" ) , KVFormat::format( "UInt" , "Mob_VI_MobVars" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Fuse working/weekend day statistics and variables of all months of a user\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_MobVar_fusiontotalvars::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_MobVar_indVars_out : MRParserOut {

	public:
	char * sep ;


	//Constructor
	MACRO_mobmx_MobVar_indVars_out() : MRParserOut( KVFormat::format( "UInt" , "Mob_VI_MobVars" ) )
	{
	}

	//Main function to implement
	int parseOut( KVSet vector , FILE *file);
	int parseOutFinish( FILE *file);

	std::string help(){
		ostringstream o;
		o<<"Parse out of individual variables of a client\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//int MACRO_mobmx_MobVar_indVars_out::parseOut( KVSet kvSet , FILE *file){}
	//int MACRO_mobmx_MobVar_indVars_out::parseOutFinish( FILE *file){}
};

class MACRO_mobmx_MobVar_delete_period : MACRO_Map {

	public:


	//Constructor
	MACRO_mobmx_MobVar_delete_period() : MACRO_Map( KVFormat::format( "TelMonth" , "CellMx" ) , KVFormat::format( "TelMonth" , "CellMx" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Delete the period to calculate individual variables of a whole sample\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_MobVar_delete_period::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_MobVar_indVars_accumulated_out : MRParserOut {

	public:
	char * sep ;


	//Constructor
	MACRO_mobmx_MobVar_indVars_accumulated_out() : MRParserOut( KVFormat::format( "UInt" , "Mob_VI_MobVars" ) )
	{
	}

	//Main function to implement
	int parseOut( KVSet vector , FILE *file);
	int parseOutFinish( FILE *file);

	std::string help(){
		ostringstream o;
		o<<"Parse out of individual variables of a client\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//int MACRO_mobmx_MobVar_indVars_accumulated_out::parseOut( KVSet kvSet , FILE *file){}
	//int MACRO_mobmx_MobVar_indVars_accumulated_out::parseOutFinish( FILE *file){}
};

class MACRO_mobmx_itin_join_cell_bts : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_itin_join_cell_bts() : MACRO_Reduce( KVFormat::vectorOfFormats(KVFormat::format( "UInt" , "CdrMx" ),KVFormat::format( "UInt" , "CellMx" )) , KVFormat::format( "TwoInt" , "ItinTime" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Change cell of cdrs by bts for itineraries\n";
		o<<"Input 1: Cdrs with cell info\n";
		o<<"Input 2: Cells catalogue\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_itin_join_cell_bts::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_itin_filter_pois : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_itin_filter_pois() : MACRO_Reduce( KVFormat::vectorOfFormats(KVFormat::format( "TwoInt" , "ItinTime" ),KVFormat::format( "TwoInt" , "Poi" )) , KVFormat::format( "UInt" , "ItinTime" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Filter cdrs to the pois of a client\n";
		o<<"Input 1: Cdrs with bts info\n";
		o<<"Input 2: Pois of clients\n";
		o<<"Output 1: Cdrs with POI\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_itin_filter_pois::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_itin_move_client_pois : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_itin_move_client_pois() : MACRO_Reduce( KVFormat::format( "UInt" , "ItinTime" ) , KVFormat::format( "UInt" , "ItinMovement" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Extract movements between POIs of a client\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_itin_move_client_pois::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_itin_get_ranges : MACRO_Map {

	public:


	//Constructor
	MACRO_mobmx_itin_get_ranges() : MACRO_Map( KVFormat::format( "UInt" , "ItinMovement" ) , KVFormat::format( "ItinRange" , "Double" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Set ranges of a POIs of a client\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_itin_get_ranges::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_itin_count_ranges : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_itin_count_ranges() : MACRO_Reduce( KVFormat::format( "ItinRange" , "Double" ) , KVFormat::format( "ItinRange" , "ItinPercMove" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Count percentages of movements in a range\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_itin_count_ranges::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_itin_get_vector : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_itin_get_vector() : MACRO_Reduce( KVFormat::format( "ItinRange" , "ItinPercMove" ) , KVFormat::format( "ItinRange" , "ClusterVector" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Get vector of moves between POIs\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_itin_get_vector::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_itin_get_itinerary : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_itin_get_itinerary() : MACRO_Reduce( KVFormat::format( "ItinRange" , "ClusterVector" ) , KVFormat::format( "UInt" , "Itinerary" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Get peaks of moves vector\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_itin_get_itinerary::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_itin_itinerary_out : MRParserOut {

	public:
	char * sep ;


	//Constructor
	MACRO_mobmx_itin_itinerary_out() : MRParserOut( KVFormat::format( "UInt" , "Itinerary" ) )
	{
	}

	//Main function to implement
	int parseOut( KVSet vector , FILE *file);
	int parseOutFinish( FILE *file);

	std::string help(){
		ostringstream o;
		o<<"Parse out of individual variables of a client\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//int MACRO_mobmx_itin_itinerary_out::parseOut( KVSet kvSet , FILE *file){}
	//int MACRO_mobmx_itin_itinerary_out::parseOutFinish( FILE *file){}
};

class MACRO_mobmx_matrix_get_group_catalogue : MRParser {

	public:


	//Constructor
	MACRO_mobmx_matrix_get_group_catalogue() : MRParser( KVFormat::format( "UInt" , "TwoInt" ) )
	{
	}

	//Main function to implement
	void parseLines( std::vector<char*> lines , bool eof, KVWriterInterface *writer );

	std::string help(){
		ostringstream o;
		o<<"Parse a txt file to extract catalogue bts-group\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_matrix_get_group_catalogue::parseLines( std::vector<char*> lines , bool eof, KVWriterInterface *writer ){}
};

class MACRO_mobmx_matrix_join_cell_group : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_matrix_join_cell_group() : MACRO_Reduce( KVFormat::vectorOfFormats(KVFormat::format( "UInt" , "CdrMx" ),KVFormat::format( "UInt" , "TwoInt" )) , KVFormat::format( "UInt" , "MatrixTime" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Change cell of cdrs by state for matrix\n";
		o<<"Input 1: Cdrs with cell info\n";
		o<<"Input 2: Cells catalogue\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_matrix_join_cell_group::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_matrix_move_client : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_matrix_move_client() : MACRO_Reduce( KVFormat::format( "UInt" , "MatrixTime" ) , KVFormat::format( "UInt" , "ItinMovement" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Extract movements between POIs of a client\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_matrix_move_client::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_matrix_get_ranges : MACRO_Map {

	public:


	//Constructor
	MACRO_mobmx_matrix_get_ranges() : MACRO_Map( KVFormat::format( "UInt" , "ItinMovement" ) , KVFormat::format( "MatrixRange" , "Double" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Set ranges of a moves of a client\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_matrix_get_ranges::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_matrix_count_ranges : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_matrix_count_ranges() : MACRO_Reduce( KVFormat::format( "MatrixRange" , "Double" ) , KVFormat::format( "ItinRange" , "ItinPercMove" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Count percentages of movements in a range\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_matrix_count_ranges::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_matrix_get_vector : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_matrix_get_vector() : MACRO_Reduce( KVFormat::format( "ItinRange" , "ItinPercMove" ) , KVFormat::format( "TwoInt" , "ClusterVector" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Get vector of moves between btss\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_matrix_get_vector::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_matrix_join_bts1_group : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_matrix_join_bts1_group() : MACRO_Reduce( KVFormat::vectorOfFormats(KVFormat::format( "UInt" , "PairIdMtx" ),KVFormat::format( "UInt" , "UInt" )) , KVFormat::format( "UInt" , "PairIdMtx" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Join first bts to group\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_matrix_join_bts1_group::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_matrix_join_bts2_group : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_matrix_join_bts2_group() : MACRO_Reduce( KVFormat::vectorOfFormats(KVFormat::format( "UInt" , "PairIdMtx" ),KVFormat::format( "UInt" , "UInt" )) , KVFormat::format( "TwoInt" , "PairIdMtx" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Join second bts to group\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_matrix_join_bts2_group::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_matrix_sum_group_vectors : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_matrix_sum_group_vectors() : MACRO_Reduce( KVFormat::format( "TwoInt" , "PairIdMtx" ) , KVFormat::format( "TwoInt" , "ClusterVector" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Sum of coms at the same groups vector\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_matrix_sum_group_vectors::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_matrix_borrar_celltobts : MACRO_Map {

	public:


	//Constructor
	MACRO_mobmx_matrix_borrar_celltobts() : MACRO_Map( KVFormat::format( "UInt" , "CellMx" ) , KVFormat::format( "UInt" , "UInt" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Convert cell catalogue to bts-state\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_matrix_borrar_celltobts::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_matrix_spread_node : MACRO_Map {

	public:


	//Constructor
	MACRO_mobmx_matrix_spread_node() : MACRO_Map( KVFormat::format( "TwoInt" , "ItinTime" ) , KVFormat::format( "UInt" , "ItinTime" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Spread cdrs info with time by node\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_matrix_spread_node::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_matrix_spread_dist_moves_by_pair : MACRO_Map {

	public:


	//Constructor
	MACRO_mobmx_matrix_spread_dist_moves_by_pair() : MACRO_Map( KVFormat::format( "ItinRange" , "Double" ) , KVFormat::format( "ItinRange" , "Double" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Spread distribution of moves by pair\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_matrix_spread_dist_moves_by_pair::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_matrix_spread_vector_by_pair : MACRO_Map {

	public:


	//Constructor
	MACRO_mobmx_matrix_spread_vector_by_pair() : MACRO_Map( KVFormat::format( "ItinRange" , "ClusterVector" ) , KVFormat::format( "TwoInt" , "ClusterVector" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Spread vector by pair of bts\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_matrix_spread_vector_by_pair::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_parse_client_profile : MRParser {

	public:


	//Constructor
	MACRO_mobmx_parse_client_profile() : MRParser( KVFormat::format( "UInt" , "UInt" ) )
	{
	}

	//Main function to implement
	void parseLines( std::vector<char*> lines , bool eof, KVWriterInterface *writer );

	std::string help(){
		ostringstream o;
		o<<"Parse a txt file to extract client profile\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_parse_client_profile::parseLines( std::vector<char*> lines , bool eof, KVWriterInterface *writer ){}
};

class MACRO_mobmx_matrix_spread_moves_node : MACRO_Map {

	public:


	//Constructor
	MACRO_mobmx_matrix_spread_moves_node() : MACRO_Map( KVFormat::format( "ItinRange" , "Double" ) , KVFormat::format( "UInt" , "ProfileMtx" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Spread parts of movements between pair of btss by node\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_matrix_spread_moves_node::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_matrix_join_moves_profile : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_matrix_join_moves_profile() : MACRO_Reduce( KVFormat::vectorOfFormats(KVFormat::format( "UInt" , "ProfileMtx" ),KVFormat::format( "UInt" , "UInt" )) , KVFormat::format( "ItinRange" , "Double" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Join user moves to user profile\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_matrix_join_moves_profile::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_matrix_get_out : MRParserOut {

	public:
	char * sep ;


	//Constructor
	MACRO_mobmx_matrix_get_out() : MRParserOut( KVFormat::format( "TwoInt" , "ClusterVector" ) )
	{
	}

	//Main function to implement
	int parseOut( KVSet vector , FILE *file);
	int parseOutFinish( FILE *file);

	std::string help(){
		ostringstream o;
		o<<"Parse out of aggregated mobility matrixes\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//int MACRO_mobmx_matrix_get_out::parseOut( KVSet kvSet , FILE *file){}
	//int MACRO_mobmx_matrix_get_out::parseOutFinish( FILE *file){}
};

class MACRO_mobmx_matrix_profile_get_out : MRParserOut {

	public:
	char * sep ;


	//Constructor
	MACRO_mobmx_matrix_profile_get_out() : MRParserOut( KVFormat::format( "ItinRange" , "ClusterVector" ) )
	{
	}

	//Main function to implement
	int parseOut( KVSet vector , FILE *file);
	int parseOutFinish( FILE *file);

	std::string help(){
		ostringstream o;
		o<<"Parse out of aggregated mobility matrixes with a profile variable\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//int MACRO_mobmx_matrix_profile_get_out::parseOut( KVSet kvSet , FILE *file){}
	//int MACRO_mobmx_matrix_profile_get_out::parseOutFinish( FILE *file){}
};

class MACRO_mobmx_popden_join_array_profile : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_popden_join_array_profile() : MACRO_Reduce( KVFormat::vectorOfFormats(KVFormat::format( "UInt" , "NodeMx_Counter" ),KVFormat::format( "UInt" , "UInt" )) , KVFormat::format( "BtsProfile" , "UInt" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Join comms array of clients to clients profile\n";
		o<<"Input 1: Array of comms\n";
		o<<"Input 2: Profile\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_popden_join_array_profile::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_popden_spread_array : MACRO_Map {

	public:


	//Constructor
	MACRO_mobmx_popden_spread_array() : MACRO_Map( KVFormat::format( "UInt" , "NodeMx_Counter" ) , KVFormat::format( "BtsProfile" , "UInt" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Spread comms array of client to clients profile\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_popden_spread_array::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_popden_sum_comms : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_popden_sum_comms() : MACRO_Reduce( KVFormat::format( "BtsProfile" , "UInt" ) , KVFormat::format( "TwoInt" , "Bts_Counter" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Sum communication by bts, profile, day of week and hour\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_popden_sum_comms::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_popden_create_vector : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_popden_create_vector() : MACRO_Reduce( KVFormat::format( "TwoInt" , "Bts_Counter" ) , KVFormat::format( "TwoInt" , "ClusterVector" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Create the vector of comms by bts and profile\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_popden_create_vector::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_popden_get_out : MRParserOut {

	public:
	char * sep ;


	//Constructor
	MACRO_mobmx_popden_get_out() : MRParserOut( KVFormat::format( "TwoInt" , "ClusterVector" ) )
	{
	}

	//Main function to implement
	int parseOut( KVSet vector , FILE *file);
	int parseOutFinish( FILE *file);

	std::string help(){
		ostringstream o;
		o<<"Parse out of density of population by bts\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//int MACRO_mobmx_popden_get_out::parseOut( KVSet kvSet , FILE *file){}
	//int MACRO_mobmx_popden_get_out::parseOutFinish( FILE *file){}
};

class MACRO_mobmx_popden_profile_get_out : MRParserOut {

	public:
	char * sep ;


	//Constructor
	MACRO_mobmx_popden_profile_get_out() : MRParserOut( KVFormat::format( "TwoInt" , "ClusterVector" ) )
	{
	}

	//Main function to implement
	int parseOut( KVSet vector , FILE *file);
	int parseOutFinish( FILE *file);

	std::string help(){
		ostringstream o;
		o<<"Parse out of density of population by profile and bts\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//int MACRO_mobmx_popden_profile_get_out::parseOut( KVSet kvSet , FILE *file){}
	//int MACRO_mobmx_popden_profile_get_out::parseOutFinish( FILE *file){}
};

class MACRO_mobmx_popden_spread_nodebtsdayhour : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_popden_spread_nodebtsdayhour() : MACRO_Reduce( KVFormat::vectorOfFormats(KVFormat::format( "UInt" , "CdrMx" ),KVFormat::format( "UInt" , "CellMx" )) , KVFormat::format( "NodBtsDate" , "Void" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Spread cdrs with location info by node, bts, day of month and hour\n";
		o<<"Input 1: cdrs with location info\n";
		o<<"Input 2: cells catalogue\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_popden_spread_nodebtsdayhour::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_popden_delete_duplicates : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_popden_delete_duplicates() : MACRO_Reduce( KVFormat::format( "NodBtsDate" , "Void" ) , KVFormat::format( "UInt" , "Node_Bts" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Delete duplicates of calls a client in an hour at the same bts\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_popden_delete_duplicates::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_popden_join_nodeinfo_profile : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_popden_join_nodeinfo_profile() : MACRO_Reduce( KVFormat::vectorOfFormats(KVFormat::format( "UInt" , "Node_Bts" ),KVFormat::format( "UInt" , "UInt" )) , KVFormat::format( "BtsProfile" , "UInt" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Join nodeinfo and node profile\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_popden_join_nodeinfo_profile::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_popden_join_nodeinfo_withoutprofile : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_popden_join_nodeinfo_withoutprofile() : MACRO_Reduce( KVFormat::format( "UInt" , "Node_Bts" ) , KVFormat::format( "BtsProfile" , "UInt" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Put 0 in profile\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_popden_join_nodeinfo_withoutprofile::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_borrar_get_bts_vector : MRParser {

	public:


	//Constructor
	MACRO_mobmx_borrar_get_bts_vector() : MRParser( KVFormat::format( "Node_Bts" , "ClusterVector" ) )
	{
	}

	//Main function to implement
	void parseLines( std::vector<char*> lines , bool eof, KVWriterInterface *writer );

	std::string help(){
		ostringstream o;
		o<<"Parse a txt file to extract bts vectors from Rocío\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_borrar_get_bts_vector::parseLines( std::vector<char*> lines , bool eof, KVWriterInterface *writer ){}
};

class MACRO_mobmx_borrar_get_bts_comarea : MRParser {

	public:


	//Constructor
	MACRO_mobmx_borrar_get_bts_comarea() : MRParser( KVFormat::format( "UInt" , "Bts" ) )
	{
	}

	//Main function to implement
	void parseLines( std::vector<char*> lines , bool eof, KVWriterInterface *writer );

	std::string help(){
		ostringstream o;
		o<<"Parse a txt file to extract bts area\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_borrar_get_bts_comarea::parseLines( std::vector<char*> lines , bool eof, KVWriterInterface *writer ){}
};

class MACRO_mobmx_borrar_get_pospoi_inout : MRParserOut {

	public:
	char * sep ;


	//Constructor
	MACRO_mobmx_borrar_get_pospoi_inout() : MRParserOut( KVFormat::format( "UInt" , "PoiPos" ) )
	{
	}

	//Main function to implement
	int parseOut( KVSet vector , FILE *file);
	int parseOutFinish( FILE *file);

	std::string help(){
		ostringstream o;
		o<<"Parse out of density of population by profile and bts\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//int MACRO_mobmx_borrar_get_pospoi_inout::parseOut( KVSet kvSet , FILE *file){}
	//int MACRO_mobmx_borrar_get_pospoi_inout::parseOutFinish( FILE *file){}
};

class MACRO_mobmx_cells_spread_GMT_to_UTM : MACRO_Map {

	public:


	//Constructor
	MACRO_mobmx_cells_spread_GMT_to_UTM() : MACRO_Map( KVFormat::format( "UInt" , "CellMx" ) , KVFormat::format( "UInt" , "CellMx" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Spread list of cells only in one group in order to convert GMT coordinates to UTM\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_cells_spread_GMT_to_UTM::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_get_husos_cells_GMT_to_UTM : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_get_husos_cells_GMT_to_UTM() : MACRO_Reduce( KVFormat::format( "UInt" , "CellMx" ) , KVFormat::format( "UInt" , "Huso" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Spread with huso for input cells\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_get_husos_cells_GMT_to_UTM::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_calc_cells_GMT_to_UTM : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_calc_cells_GMT_to_UTM() : MACRO_Reduce( KVFormat::vectorOfFormats(KVFormat::format( "UInt" , "CellMx" ),KVFormat::format( "UInt" , "Huso" )) , KVFormat::format( "UInt" , "CellMxUTM" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Calculate the UTM coordinates\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_calc_cells_GMT_to_UTM::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_calc_cells_UTM_to_GMT : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_calc_cells_UTM_to_GMT() : MACRO_Reduce( KVFormat::format( "UInt" , "CellMxUTM" ) , KVFormat::format( "UInt" , "CellMx" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Calculate the GMT coordinates\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_calc_cells_UTM_to_GMT::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_calc_bts_UTM_to_GMT : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_calc_bts_UTM_to_GMT() : MACRO_Reduce( KVFormat::format( "UInt" , "Bts" ) , KVFormat::format( "UInt" , "Bts" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Calculate the GMT coordinates\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_calc_bts_UTM_to_GMT::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_calc_vertices_UTM_to_GMT : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_calc_vertices_UTM_to_GMT() : MACRO_Reduce( KVFormat::format( "UInt" , "Vertices_Voronoi" ) , KVFormat::format( "UInt" , "Vertices_Voronoi" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Calculate the GMT coordinates\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_calc_vertices_UTM_to_GMT::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_bts_spread : MACRO_Map {

	public:


	//Constructor
	MACRO_mobmx_bts_spread() : MACRO_Map( KVFormat::format( "UInt" , "CellMxUTM" ) , KVFormat::format( "UInt" , "bts_coords" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Spread list of bts and coordinates\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_bts_spread::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_bts_norepeat : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_bts_norepeat() : MACRO_Reduce( KVFormat::format( "UInt" , "bts_coords" ) , KVFormat::format( "UInt" , "bts_coords" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Remove repeated bts\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_bts_norepeat::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_bts_spread_voronoi : MACRO_Map {

	public:


	//Constructor
	MACRO_mobmx_bts_spread_voronoi() : MACRO_Map( KVFormat::format( "UInt" , "bts_coords" ) , KVFormat::format( "UInt" , "bts_coords" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Spread list of cells only in one group in order to calculate Voronoi's polygons\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_bts_spread_voronoi::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_calc_voronoi : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_calc_voronoi() : MACRO_Reduce( KVFormat::vectorOfFormats(KVFormat::format( "UInt" , "bts_coords" ),KVFormat::format( "UInt" , "Contorno" )) , KVFormat::vectorOfFormats(KVFormat::format( "UInt" , "Bts" ),KVFormat::format( "UInt" , "Vertices_Voronoi" )) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Calculate the Voronoi's Polygons for a group of cells\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_calc_voronoi::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_borrar_get_examples : MACRO_Map {

	public:


	//Constructor
	MACRO_mobmx_borrar_get_examples() : MACRO_Map( KVFormat::format( "UInt" , "CdrMx" ) , KVFormat::format( "UInt" , "CdrMx" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"Extract cdrs where a set of phones are involved\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_borrar_get_examples::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_borrar_get_nclient_with_itineraries : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_borrar_get_nclient_with_itineraries() : MACRO_Reduce( KVFormat::format( "UInt" , "Itinerary" ) , KVFormat::vectorOfFormats(KVFormat::format( "UInt" , "Void" ),KVFormat::format( "TelMonth" , "Itinerary" )) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_borrar_get_nclient_with_itineraries::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_borrar_delete_itindup : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_borrar_delete_itindup() : MACRO_Reduce( KVFormat::format( "TelMonth" , "Itinerary" ) , KVFormat::format( "UInt" , "TwoInt" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_borrar_delete_itindup::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_borrar_get_numItin : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_borrar_get_numItin() : MACRO_Reduce( KVFormat::format( "UInt" , "TwoInt" ) , KVFormat::format( "UInt" , "Void" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_borrar_get_numItin::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

class MACRO_mobmx_borrar_get_statistics : MACRO_Reduce {

	public:


	//Constructor
	MACRO_mobmx_borrar_get_statistics() : MACRO_Reduce( KVFormat::format( "UInt" , "Void" ) , KVFormat::format( "UInt" , "UInt" ) )
	{
	}

	//Main function to implement
	void run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs);

	std::string help(){
		ostringstream o;
		o<<"\n";
		return o.str();
	}

	
	//Please, copy and paste from here to implement specific functions
	//void MACRO_mobmx_borrar_get_statistics::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs){}
};

//Module definition

class MACRO_Module_TEST : public MACRO_Module{

public:
	MACRO_Module_TEST(){
		name	= "TEST";
		version = "1.0";
		auth	= "Alberto Martín";


		//Add datas
		add("datas","TwoInt" , au::factory<MRData_TwoInt> );
		add("datas","Huso" , au::factory<MRData_Huso> );
		add("datas","CdrMx" , au::factory<MRData_CdrMx> );
		add("datas","CellMx" , au::factory<MRData_CellMx> );
		add("datas","Bts" , au::factory<MRData_Bts> );
		add("datas","Poi" , au::factory<MRData_Poi> );
		add("datas","Node_Bts" , au::factory<MRData_Node_Bts> );
		add("datas","Bts_Counter" , au::factory<MRData_Bts_Counter> );
		add("datas","NodeMx_Counter" , au::factory<MRData_NodeMx_Counter> );
		add("datas","Node_Bts_Day" , au::factory<MRData_Node_Bts_Day> );
		add("datas","Double" , au::factory<MRData_Double> );
		add("datas","DateVector" , au::factory<MRData_DateVector> );
		add("datas","DailyVector" , au::factory<MRData_DailyVector> );
		add("datas","ClusterVector" , au::factory<MRData_ClusterVector> );
		add("datas","Cluster" , au::factory<MRData_Cluster> );
		add("datas","ClusterSet" , au::factory<MRData_ClusterSet> );
		add("datas","PoiPos" , au::factory<MRData_PoiPos> );
		add("datas","PoiNew" , au::factory<MRData_PoiNew> );
		add("datas","PoiAdj" , au::factory<MRData_PoiAdj> );
		add("datas","VectorPairs" , au::factory<MRData_VectorPairs> );
		add("datas","TelMonth" , au::factory<MRData_TelMonth> );
		add("datas","TempMob" , au::factory<MRData_TempMob> );
		add("datas","MobVars" , au::factory<MRData_MobVars> );
		add("datas","Mob_VI_MobVars" , au::factory<MRData_Mob_VI_MobVars> );
		add("datas","ItinTime" , au::factory<MRData_ItinTime> );
		add("datas","ItinMovement" , au::factory<MRData_ItinMovement> );
		add("datas","ItinRange" , au::factory<MRData_ItinRange> );
		add("datas","ItinPercMove" , au::factory<MRData_ItinPercMove> );
		add("datas","Itinerary" , au::factory<MRData_Itinerary> );
		add("datas","MatrixTime" , au::factory<MRData_MatrixTime> );
		add("datas","MatrixRange" , au::factory<MRData_MatrixRange> );
		add("datas","PairIdMtx" , au::factory<MRData_PairIdMtx> );
		add("datas","ProfileMtx" , au::factory<MRData_ProfileMtx> );
		add("datas","NodBtsDate" , au::factory<MRData_NodBtsDate> );
		add("datas","BtsProfile" , au::factory<MRData_BtsProfile> );
		add("datas","Elipsoide" , au::factory<MRData_Elipsoide> );
		add("datas","CellMxUTM" , au::factory<MRData_CellMxUTM> );
		add("datas","bts_coords" , au::factory<MRData_bts_coords> );
		add("datas","Vertices_Voronoi" , au::factory<MRData_Vertices_Voronoi> );
		add("datas","Contorno" , au::factory<MRData_Contorno> );


		//Add operatons
		add("operations","mob_script_parse_data" , au::factory<MACRO_mob_script_parse_data> );
		add("operations","mob_script_prepare_data" , au::factory<MACRO_mob_script_prepare_data> );
		add("operations","mob_script_get_vectors_to_cluster" , au::factory<MACRO_mob_script_get_vectors_to_cluster> );
		add("operations","mob_script_extract_MIVs" , au::factory<MACRO_mob_script_extract_MIVs> );
		add("operations","mob_script_extract_POIs" , au::factory<MACRO_mob_script_extract_POIs> );
		add("operations","mob_script_label_POIs" , au::factory<MACRO_mob_script_label_POIs> );
		add("operations","mob_script_label_client" , au::factory<MACRO_mob_script_label_client> );
		add("operations","mob_script_label_bts" , au::factory<MACRO_mob_script_label_bts> );
		add("operations","mob_script_label_client_bts" , au::factory<MACRO_mob_script_label_client_bts> );
		add("operations","mob_script_join_labels" , au::factory<MACRO_mob_script_join_labels> );
		add("operations","mob_script_detect_second_homes" , au::factory<MACRO_mob_script_detect_second_homes> );
		add("operations","mob_script_out_pois" , au::factory<MACRO_mob_script_out_pois> );
		add("operations","mob_script_get_itineraries" , au::factory<MACRO_mob_script_get_itineraries> );
		add("operations","mob_script_get_population_density" , au::factory<MACRO_mob_script_get_population_density> );
		add("operations","mob_script_get_population_density_profile" , au::factory<MACRO_mob_script_get_population_density_profile> );
		add("operations","mobmx_parse_cdrs" , au::factory<MACRO_mobmx_parse_cdrs> );
		add("operations","mobmx_get_sample_10000" , au::factory<MACRO_mobmx_get_sample_10000> );
		add("operations","mobmx_parse_cells" , au::factory<MACRO_mobmx_parse_cells> );
		add("operations","mobmx_parse_contorno" , au::factory<MACRO_mobmx_parse_contorno> );
		add("operations","mobmx_parse_client_clusters" , au::factory<MACRO_mobmx_parse_client_clusters> );
		add("operations","mobmx_parse_clientbts_clusters" , au::factory<MACRO_mobmx_parse_clientbts_clusters> );
		add("operations","mobmx_parse_bts_clusters" , au::factory<MACRO_mobmx_parse_bts_clusters> );
		add("operations","mobmx_get_mob_from_cdrs" , au::factory<MACRO_mobmx_get_mob_from_cdrs> );
		add("operations","mobmx_get_mob_from_cells" , au::factory<MACRO_mobmx_get_mob_from_cells> );
		add("operations","mobmx_filter_cellnoinfo" , au::factory<MACRO_mobmx_filter_cellnoinfo> );
		add("operations","mobmx_join_bts_node" , au::factory<MACRO_mobmx_join_bts_node> );
		add("operations","mobmx_node_bts_counter" , au::factory<MACRO_mobmx_node_bts_counter> );
		add("operations","mobmx_node_mobInfo" , au::factory<MACRO_mobmx_node_mobInfo> );
		add("operations","mobmx_repbts_spread_nodebts" , au::factory<MACRO_mobmx_repbts_spread_nodebts> );
		add("operations","mobmx_repbts_aggbybts" , au::factory<MACRO_mobmx_repbts_aggbybts> );
		add("operations","mobmx_repbts_filter_num_comms" , au::factory<MACRO_mobmx_repbts_filter_num_comms> );
		add("operations","mobmx_repbts_join_dist_comms" , au::factory<MACRO_mobmx_repbts_join_dist_comms> );
		add("operations","mobmx_repbts_get_representative_bts" , au::factory<MACRO_mobmx_repbts_get_representative_bts> );
		add("operations","mobmx_vector_normalized" , au::factory<MACRO_mobmx_vector_normalized> );
		add("operations","mobmx_vector_filt_clients" , au::factory<MACRO_mobmx_vector_filt_clients> );
		add("operations","mobmx_vector_spread_nodedayhour" , au::factory<MACRO_mobmx_vector_spread_nodedayhour> );
		add("operations","mobmx_vector_get_ncoms_nodedayhour" , au::factory<MACRO_mobmx_vector_get_ncoms_nodedayhour> );
		add("operations","mobmx_vector_create_node_dayhour" , au::factory<MACRO_mobmx_vector_create_node_dayhour> );
		add("operations","mobmx_vector_fuse_node_daygroup" , au::factory<MACRO_mobmx_vector_fuse_node_daygroup> );
		add("operations","mobmx_vector_spread_nodbts" , au::factory<MACRO_mobmx_vector_spread_nodbts> );
		add("operations","mobmx_vector_sum_groupcomms" , au::factory<MACRO_mobmx_vector_sum_groupcomms> );
		add("operations","mobmx_vector_filt_clientbts" , au::factory<MACRO_mobmx_vector_filt_clientbts> );
		add("operations","mobmx_vector_filter_bts" , au::factory<MACRO_mobmx_vector_filter_bts> );
		add("operations","mobmx_vector_sum_coms_bts" , au::factory<MACRO_mobmx_vector_sum_coms_bts> );
		add("operations","mobmx_cluster_client_get_min_distance" , au::factory<MACRO_mobmx_cluster_client_get_min_distance> );
		add("operations","mobmx_cluster_clientbts_get_min_distance" , au::factory<MACRO_mobmx_cluster_clientbts_get_min_distance> );
		add("operations","mobmx_cluster_bts_get_min_distance" , au::factory<MACRO_mobmx_cluster_bts_get_min_distance> );
		add("operations","mobmx_filter_bts_vector" , au::factory<MACRO_mobmx_filter_bts_vector> );
		add("operations","mobmx_cluster_agg_node_cluster" , au::factory<MACRO_mobmx_cluster_agg_node_cluster> );
		add("operations","mobmx_cluster_spread_nodelbl_poilbl" , au::factory<MACRO_mobmx_cluster_spread_nodelbl_poilbl> );
		add("operations","mobmx_cluster_count_majPoi_by_node" , au::factory<MACRO_mobmx_cluster_count_majPoi_by_node> );
		add("operations","mobmx_cluster_get_majPoi_by_node" , au::factory<MACRO_mobmx_cluster_get_majPoi_by_node> );
		add("operations","mobmx_cluster_join_potPoi_label" , au::factory<MACRO_mobmx_cluster_join_potPoi_label> );
		add("operations","mobmx_cluster_agg_potPoi_pois" , au::factory<MACRO_mobmx_cluster_agg_potPoi_pois> );
		add("operations","mobmx_cluster_agg_bts_cluster" , au::factory<MACRO_mobmx_cluster_agg_bts_cluster> );
		add("operations","mobmx_poi_cell_to_bts" , au::factory<MACRO_mobmx_poi_cell_to_bts> );
		add("operations","mobmx_poi_join_pois_btscoord" , au::factory<MACRO_mobmx_poi_join_pois_btscoord> );
		add("operations","mobmx_poi_join_pois_vi" , au::factory<MACRO_mobmx_poi_join_pois_vi> );
		add("operations","mobmx_poi_get_pairs_sechome_pois" , au::factory<MACRO_mobmx_poi_get_pairs_sechome_pois> );
		add("operations","mobmx_poi_filter_sechome_adjacent" , au::factory<MACRO_mobmx_poi_filter_sechome_adjacent> );
		add("operations","mobmx_poi_delete_sechome_duplicate" , au::factory<MACRO_mobmx_poi_delete_sechome_duplicate> );
		add("operations","mobmx_poi_join_sechome_results" , au::factory<MACRO_mobmx_poi_join_sechome_results> );
		add("operations","mobmx_adj_parse_adj_bts" , au::factory<MACRO_mobmx_adj_parse_adj_bts> );
		add("operations","mobmx_adj_add_unique_id_poi" , au::factory<MACRO_mobmx_adj_add_unique_id_poi> );
		add("operations","mobmx_adj_group_type_poi_client" , au::factory<MACRO_mobmx_adj_group_type_poi_client> );
		add("operations","mobmx_adj_join_pairbts_adjbts" , au::factory<MACRO_mobmx_adj_join_pairbts_adjbts> );
		add("operations","mobmx_adj_spread_count" , au::factory<MACRO_mobmx_adj_spread_count> );
		add("operations","mobmx_adj_count_indexes" , au::factory<MACRO_mobmx_adj_count_indexes> );
		add("operations","mobmx_adj_put_max_id" , au::factory<MACRO_mobmx_adj_put_max_id> );
		add("operations","mobmx_adj_update_pois_table" , au::factory<MACRO_mobmx_adj_update_pois_table> );
		add("operations","mobmx_adj_swap_poiId_st1" , au::factory<MACRO_mobmx_adj_swap_poiId_st1> );
		add("operations","mobmx_adj_swap_poiId_st2" , au::factory<MACRO_mobmx_adj_swap_poiId_st2> );
		add("operations","mobmx_adj_spread_table_by_poiId" , au::factory<MACRO_mobmx_adj_spread_table_by_poiId> );
		add("operations","mobmx_adj_spread_pois_by_poiId" , au::factory<MACRO_mobmx_adj_spread_pois_by_poiId> );
		add("operations","mobmx_adj_join_new_poiId" , au::factory<MACRO_mobmx_adj_join_new_poiId> );
		add("operations","mobmx_adj_change_poisId" , au::factory<MACRO_mobmx_adj_change_poisId> );
		add("operations","mobmx_poi_spread_nodebts_vector" , au::factory<MACRO_mobmx_poi_spread_nodebts_vector> );
		add("operations","mobmx_poi_join_poivector_poi" , au::factory<MACRO_mobmx_poi_join_poivector_poi> );
		add("operations","mobmx_poi_normalize_poi_vector" , au::factory<MACRO_mobmx_poi_normalize_poi_vector> );
		add("operations","mobmx_vector_oneid_out" , au::factory<MACRO_mobmx_vector_oneid_out> );
		add("operations","mobmx_vector_nodbts_out" , au::factory<MACRO_mobmx_vector_nodbts_out> );
		add("operations","mobmx_vector_pois_out" , au::factory<MACRO_mobmx_vector_pois_out> );
		add("operations","mobmx_MobVar_calcvariables" , au::factory<MACRO_mobmx_MobVar_calcvariables> );
		add("operations","mobmx_MobVar_fusiontotalvars" , au::factory<MACRO_mobmx_MobVar_fusiontotalvars> );
		add("operations","mobmx_MobVar_indVars_out" , au::factory<MACRO_mobmx_MobVar_indVars_out> );
		add("operations","mobmx_MobVar_delete_period" , au::factory<MACRO_mobmx_MobVar_delete_period> );
		add("operations","mobmx_MobVar_indVars_accumulated_out" , au::factory<MACRO_mobmx_MobVar_indVars_accumulated_out> );
		add("operations","mobmx_itin_join_cell_bts" , au::factory<MACRO_mobmx_itin_join_cell_bts> );
		add("operations","mobmx_itin_filter_pois" , au::factory<MACRO_mobmx_itin_filter_pois> );
		add("operations","mobmx_itin_move_client_pois" , au::factory<MACRO_mobmx_itin_move_client_pois> );
		add("operations","mobmx_itin_get_ranges" , au::factory<MACRO_mobmx_itin_get_ranges> );
		add("operations","mobmx_itin_count_ranges" , au::factory<MACRO_mobmx_itin_count_ranges> );
		add("operations","mobmx_itin_get_vector" , au::factory<MACRO_mobmx_itin_get_vector> );
		add("operations","mobmx_itin_get_itinerary" , au::factory<MACRO_mobmx_itin_get_itinerary> );
		add("operations","mobmx_itin_itinerary_out" , au::factory<MACRO_mobmx_itin_itinerary_out> );
		add("operations","mobmx_matrix_get_group_catalogue" , au::factory<MACRO_mobmx_matrix_get_group_catalogue> );
		add("operations","mobmx_matrix_join_cell_group" , au::factory<MACRO_mobmx_matrix_join_cell_group> );
		add("operations","mobmx_matrix_move_client" , au::factory<MACRO_mobmx_matrix_move_client> );
		add("operations","mobmx_matrix_get_ranges" , au::factory<MACRO_mobmx_matrix_get_ranges> );
		add("operations","mobmx_matrix_count_ranges" , au::factory<MACRO_mobmx_matrix_count_ranges> );
		add("operations","mobmx_matrix_get_vector" , au::factory<MACRO_mobmx_matrix_get_vector> );
		add("operations","mobmx_matrix_join_bts1_group" , au::factory<MACRO_mobmx_matrix_join_bts1_group> );
		add("operations","mobmx_matrix_join_bts2_group" , au::factory<MACRO_mobmx_matrix_join_bts2_group> );
		add("operations","mobmx_matrix_sum_group_vectors" , au::factory<MACRO_mobmx_matrix_sum_group_vectors> );
		add("operations","mobmx_matrix_borrar_celltobts" , au::factory<MACRO_mobmx_matrix_borrar_celltobts> );
		add("operations","mobmx_matrix_spread_node" , au::factory<MACRO_mobmx_matrix_spread_node> );
		add("operations","mobmx_matrix_spread_dist_moves_by_pair" , au::factory<MACRO_mobmx_matrix_spread_dist_moves_by_pair> );
		add("operations","mobmx_matrix_spread_vector_by_pair" , au::factory<MACRO_mobmx_matrix_spread_vector_by_pair> );
		add("operations","mobmx_parse_client_profile" , au::factory<MACRO_mobmx_parse_client_profile> );
		add("operations","mobmx_matrix_spread_moves_node" , au::factory<MACRO_mobmx_matrix_spread_moves_node> );
		add("operations","mobmx_matrix_join_moves_profile" , au::factory<MACRO_mobmx_matrix_join_moves_profile> );
		add("operations","mobmx_matrix_get_out" , au::factory<MACRO_mobmx_matrix_get_out> );
		add("operations","mobmx_matrix_profile_get_out" , au::factory<MACRO_mobmx_matrix_profile_get_out> );
		add("operations","mobmx_popden_join_array_profile" , au::factory<MACRO_mobmx_popden_join_array_profile> );
		add("operations","mobmx_popden_spread_array" , au::factory<MACRO_mobmx_popden_spread_array> );
		add("operations","mobmx_popden_sum_comms" , au::factory<MACRO_mobmx_popden_sum_comms> );
		add("operations","mobmx_popden_create_vector" , au::factory<MACRO_mobmx_popden_create_vector> );
		add("operations","mobmx_popden_get_out" , au::factory<MACRO_mobmx_popden_get_out> );
		add("operations","mobmx_popden_profile_get_out" , au::factory<MACRO_mobmx_popden_profile_get_out> );
		add("operations","mobmx_popden_spread_nodebtsdayhour" , au::factory<MACRO_mobmx_popden_spread_nodebtsdayhour> );
		add("operations","mobmx_popden_delete_duplicates" , au::factory<MACRO_mobmx_popden_delete_duplicates> );
		add("operations","mobmx_popden_join_nodeinfo_profile" , au::factory<MACRO_mobmx_popden_join_nodeinfo_profile> );
		add("operations","mobmx_popden_join_nodeinfo_withoutprofile" , au::factory<MACRO_mobmx_popden_join_nodeinfo_withoutprofile> );
		add("operations","mobmx_borrar_get_bts_vector" , au::factory<MACRO_mobmx_borrar_get_bts_vector> );
		add("operations","mobmx_borrar_get_bts_comarea" , au::factory<MACRO_mobmx_borrar_get_bts_comarea> );
		add("operations","mobmx_borrar_get_pospoi_inout" , au::factory<MACRO_mobmx_borrar_get_pospoi_inout> );
		add("operations","mobmx_cells_spread_GMT_to_UTM" , au::factory<MACRO_mobmx_cells_spread_GMT_to_UTM> );
		add("operations","mobmx_get_husos_cells_GMT_to_UTM" , au::factory<MACRO_mobmx_get_husos_cells_GMT_to_UTM> );
		add("operations","mobmx_calc_cells_GMT_to_UTM" , au::factory<MACRO_mobmx_calc_cells_GMT_to_UTM> );
		add("operations","mobmx_calc_cells_UTM_to_GMT" , au::factory<MACRO_mobmx_calc_cells_UTM_to_GMT> );
		add("operations","mobmx_calc_bts_UTM_to_GMT" , au::factory<MACRO_mobmx_calc_bts_UTM_to_GMT> );
		add("operations","mobmx_calc_vertices_UTM_to_GMT" , au::factory<MACRO_mobmx_calc_vertices_UTM_to_GMT> );
		add("operations","mobmx_bts_spread" , au::factory<MACRO_mobmx_bts_spread> );
		add("operations","mobmx_bts_norepeat" , au::factory<MACRO_mobmx_bts_norepeat> );
		add("operations","mobmx_bts_spread_voronoi" , au::factory<MACRO_mobmx_bts_spread_voronoi> );
		add("operations","mobmx_calc_voronoi" , au::factory<MACRO_mobmx_calc_voronoi> );
		add("operations","mobmx_borrar_get_examples" , au::factory<MACRO_mobmx_borrar_get_examples> );
		add("operations","mobmx_borrar_get_nclient_with_itineraries" , au::factory<MACRO_mobmx_borrar_get_nclient_with_itineraries> );
		add("operations","mobmx_borrar_delete_itindup" , au::factory<MACRO_mobmx_borrar_delete_itindup> );
		add("operations","mobmx_borrar_get_numItin" , au::factory<MACRO_mobmx_borrar_get_numItin> );
		add("operations","mobmx_borrar_get_statistics" , au::factory<MACRO_mobmx_borrar_get_statistics> );
	}

	std::string help(){
		ostringstream o;
		o<<"Module TEST help\n";
		o<<"\n";
		o<<"This module is the starting point to use MACRO as developer\n";
		o<<"Please use this module to create your first map/reduce operations\n";
		return o.str();
	}
	void setEnvironment( MREnvironment *environment ){
	}
};

#endif
