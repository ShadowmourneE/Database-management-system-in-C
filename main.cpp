
#include "pch.h"
#include <iostream>
#include <stdio.h>
#include <process.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>

#pragma warning (disable : 4996)

FILE *Sfl;
FILE *Sind;
FILE *SP;

//templates for FileNames
#define file_name_fl "S.fl"
#define file_name_ind "S.ind"
#define file_name_flSp "S.fl"
//remplates for access mode in C file
#define _modeTemplate_rb "r+b"
#define _modeTemplate_wb "w+b"

const char comm_getM[] = "getM";//getM Id   
const char comm_getS[] = "getS";//getS id1 id2  
const char comm_delM[] = "delM";//delM id 
const char comm_delS[] = "delS";//delS id1 id2 
const char comm_updateM[] = "updateM";//updateM id text 
const char comm_updateS[] = "updateS";//updateS id1 id2 text 
const char comm_insertM[] = "insertM";//insertM id text 
const char comm_insertS[] = "insertS";//insertS id id2 text 


struct elementM {
	short Sadderess; //first S id
	char data[30];
};



struct elementS {
	short id;
	short isA;
	short prev;
	short next;
	char data[24];
};

int getM(short id, char* target) {
	elementM M;
	indexM Index;

	fseek(Sind, 0, SEEK_SET);
	fseek(Sfl, 0, SEEK_SET);
	do {
		if (fread(&Index, sizeof(indexM), 1, Sind) == 0) { 
			if (feof(Sind)) { return -1; }
			else exit(0); //error
		}
	} while (Index.id != id); //find

	if (Index.isA == 0) {
		return -1;
	}

	fseek(Sfl, sizeof(elementM)*Index.address, SEEK_SET); 


	if (fread(&M, sizeof(elementM), 1, Sfl) == 0) { 
		exit(0); //error
	}

	strcpy_s(target, 30, M.data);  
	return Index.id;
}

int getS(short mId, short sId, char* target) {
	elementM M;
	elementS S;
	indexM iM;
	fseek(Sind, 0, SEEK_SET);
	fseek(Sfl, 0, SEEK_SET);
	do {
		if (fread(&iM, sizeof(indexM), 1, Sind) == 0) { 
			if (feof(Sind)) return -1;
			else exit(0); //error
		}
	} while (iM.id != mId); //find

	if (iM.isA == 0) { return -1; }

	fseek(Sfl, sizeof(elementM)*iM.address, SEEK_SET); 

	if (fread(&M, sizeof(elementM), 1, Sfl) == 0)exit(0); 

	short addr = M.Sadderess;

	do {
		if (addr == -1)return -1;
		fseek(SP, sizeof(elementS)*addr, SEEK_SET); //addressing  SS
		if (fread(&S, sizeof(elementS), 1, SP) == 0)exit(0);//element get S
		addr = S.next;
	} while (S.id != sId);

	if (S.isA == 0)return -1;

	strcpy_s(target, 24, S.data);
	return S.id;
}
//видаляє запис
int delM(short mId) {
	elementM M;
	elementS S;
	indexM iM;
	fseek(Sind, 0, SEEK_SET);
	fseek(Sfl, 0, SEEK_SET);

	do {
		if (fread(&iM, sizeof(indexM), 1, Sind) == 0) { //index
			if (feof(Sind))return -1;
			else exit(0); //error
		}
	} while (iM.id != mId); //find

	if (iM.isA == 0) {
		return -1;
	}

	iM.isA = 0;
	fseek(Sind, -(long)sizeof(indexM), SEEK_CUR);
	if (fwrite(&iM, sizeof(indexM), 1, Sind) == 0)	return -1;

	fseek(Sfl, sizeof(elementM)*iM.address, SEEK_SET); //adressing


	if (fread(&M, sizeof(elementM), 1, Sfl) == 0)
		exit(0); //element get M
			//error

	short addr = M.Sadderess;

	do {
		if (addr == -1)return 1;
		fseek(SP, sizeof(elementS)*addr, SEEK_SET); //adressing  SS
		if (fread(&S, sizeof(elementS), 1, SP) == 0) { //element get S
			exit(0); //error
		}

		addr = S.next;

		S.isA = 0;
		fseek(SP, -(long)sizeof(elementS), SEEK_CUR);
		fwrite(&S, sizeof(elementS), 1, SP);

	} while (1);

	fflush(Sind);
	fflush(Sfl);
	fflush(SP);

	return 1;
}

int delS(short mId, short sId) {
	elementM M;
	elementS S;
	indexM iM;
	fseek(Sind, 0, SEEK_SET);
	fseek(Sfl, 0, SEEK_SET);
	do {
		if (fread(&iM, sizeof(indexM), 1, Sind) == 0) { //index
			if (feof(Sind)) return -1;
			else exit(0); //error
		}
	} while (iM.id != mId); //find

	if (iM.isA == 0) { return -1; }

	fseek(Sfl, sizeof(elementM)*iM.address, SEEK_SET); //adressing

	if (fread(&M, sizeof(elementM), 1, Sfl) == 0)exit(0); //element get M

	short addr = M.Sadderess;

	while (1) {
		if (addr == -1)return -1;
		fseek(SP, sizeof(elementS)*addr, SEEK_SET); //adressing  SS
		if (fread(&S, sizeof(elementS), 1, SP) == 0) { //element get S
			exit(0); //error
		}

		if (S.id == sId) break;
		addr = S.next;
	}

	if (S.isA == 0)return -1;

	S.isA = 0;
	fseek(SP, addr * sizeof(elementS), SEEK_SET);
	if (fwrite(&S, sizeof(elementS), 1, SP) == 0) return -1;

	fflush(SP);

	return S.id;
}

int updateM(short mId, char* data) {
	elementM M;
	indexM iM;

	fseek(Sind, 0, SEEK_SET);
	fseek(Sfl, 0, SEEK_SET);
	do {
		if (fread(&iM, sizeof(indexM), 1, Sind) == 0) { //index
			if (feof(Sind)) return -1;
			else exit(0); //error
		}
	} while (iM.id != mId); //find

	if (iM.isA == 0) {
		return -1;
	}

	fseek(Sfl, sizeof(elementM)*iM.address, SEEK_SET); //adressing


	if (fread(&M, sizeof(elementM), 1, Sfl) == 0) { //element get M
		exit(0); //error
	}

	strcpy_s(M.data, 30, data);  //copy data to file

	fseek(Sfl, sizeof(elementM)*iM.address, SEEK_SET); //adressing to write

	if (fwrite(&M, sizeof(elementM), 1, Sfl) == 0) {
		return -1;
	}

	fflush(Sfl);
	return iM.id;
}

int updateS(short mId, short sId, char* data) {
	elementM M;
	elementS S;
	indexM iM;
	fseek(Sind, 0, SEEK_SET);
	fseek(Sfl, 0, SEEK_SET);
	do {
		if (fread(&iM, sizeof(indexM), 1, Sind) == 0) { //index
			if (feof(Sind)) return -1;
			else exit(0); //error
		}
	} while (iM.id != mId); //find

	if (iM.isA == 0) { return -1; }

	fseek(Sfl, sizeof(elementM)*iM.address, SEEK_SET); //adressing

	if (fread(&M, sizeof(elementM), 1, Sfl) == 0)exit(0); //element get M

	short addr = M.Sadderess;

	while (1) {
		if (addr == -1)return -1;
		fseek(SP, sizeof(elementS)*addr, SEEK_SET); //adressing  SS
		if (fread(&S, sizeof(elementS), 1, SP) == 0) { //element get S
			exit(0); //error
		}

		if (S.id == sId) break;
		addr = S.next;
	}

	if (S.isA == 0)return -1;

	strcpy_s(S.data, 24, data);

	fseek(SP, addr * sizeof(elementS), SEEK_SET);
	if (fwrite(&S, sizeof(elementS), 1, SP) == 0) return -1;

	fflush(SP);

	return S.id;
}

int insertM(short mId, char * data) {
	elementM M;
	indexM iM;
	M.Sadderess = -1;
	strcpy_s(M.data, 30, data);


	int alv = 1;
	int free = -1;

	fseek(Sind, 0, SEEK_SET);
	fseek(Sfl, 0, SEEK_SET);


	do {
		if (fread(&iM, sizeof(indexM), 1, Sind) == 0) { //index
			if (feof(Sind)) { alv = 0; break; }
			else {
				int err = ferror(Sind);
				printf("%d", err);
				exit(0);
			}//error
		}

	} while (iM.id != mId); //find

	if (alv) {
		if (iM.isA == 1) return -1;
		iM.isA = 1;

		fseek(Sfl, sizeof(elementM)*iM.address, SEEK_SET); //adressing
		if (fwrite(&M, sizeof(elementM), 1, Sfl) == 0)	return -1;

		fseek(Sind, -(long)sizeof(elementM), SEEK_CUR);
		if (fwrite(&iM, sizeof(elementM), 1, Sind) == 0)	return -1;

		fflush(Sind);
		fflush(Sfl);

		return iM.address;
	}
	else {
		fseek(Sfl, 0, SEEK_END); //adressing
		if (fwrite(&M, sizeof(elementM), 1, Sfl) == 0)	return -1;

		iM.id = mId;
		iM.isA = 1;
		iM.address = ftell(Sfl) / sizeof(elementM) - 1;

		fseek(Sind, 0, SEEK_END); //adressing
		if (fwrite(&iM, sizeof(indexM), 1, Sind) == 0)	return -1;

		fflush(Sind);
		fflush(Sfl);

		return iM.address;
	}


}

int insertS(short mId, short sId, char* data) {
	elementM M;
	elementS S;
	indexM iM;

	fseek(Sind, 0, SEEK_SET);
	fseek(Sfl, 0, SEEK_SET);
	fseek(SP, 0, SEEK_SET);

	do {
		if (fread(&iM, sizeof(indexM), 1, Sind) == 0) { //index
			if (feof(Sind))return -1;
			else exit(0); //error
		}
	} while (iM.id != mId); //find

	if (iM.isA == 0)return -1;

	fseek(Sfl, sizeof(elementM)*iM.address, SEEK_SET); //adressing

	if (fread(&M, sizeof(elementM), 1, Sfl) == 0) exit(0); //element get M

	if (M.Sadderess == -1) {
		S.id = sId;
		S.isA = 1;
		S.prev = -1;
		S.next = -1;
		strcpy_s(S.data, 24, data);

		fseek(SP, 0, SEEK_END);
		if (fwrite(&S, sizeof(elementS), 1, SP) == 0)	return -1;

		M.Sadderess = ftell(SP) / sizeof(elementS) - 1;

		fseek(Sfl, sizeof(elementM)*iM.address, SEEK_SET);
		if (fwrite(&M, sizeof(elementM), 1, Sfl) == 0)	return -1;
	}
	else {
		short addr = M.Sadderess;

		while (1) {
			if (addr == -1)break;
			fseek(SP, sizeof(elementS)*addr, SEEK_SET); //adressing  SS
			if (fread(&S, sizeof(elementS), 1, SP) == 0) { //element get S
				exit(0); //error
			}

			if (S.id == sId) break;
			addr = S.next;
		}

		if (addr != -1) { //replace element
			if (S.isA == 1)return -1;

			S.id = sId;
			S.isA = 1;
			strcpy_s(S.data, 24, data);
			fseek(SP, addr * sizeof(elementS), SEEK_SET);

			if (fwrite(&S, sizeof(elementS), 1, SP) == 0)	return -1;
		}
		else {
			addr = M.Sadderess;

			S.id = sId;
			S.isA = 1;
			strcpy_s(S.data, 24, data);
			S.next = addr;
			S.prev = -1;

			fseek(SP, 0, SEEK_END);
			if (fwrite(&S, sizeof(elementS), 1, SP) == 0) return -1;

			M.Sadderess = ftell(SP) / sizeof(elementS) - 1;

			fseek(Sfl, sizeof(elementM)*iM.address, SEEK_SET);
			if (fwrite(&M, sizeof(elementM), 1, Sfl) == 0)	return -1;

		}

	}

	fflush(SP);
	fflush(Sfl);

	return 0;
}
//sHOW CONSOLE interface
void showInterface() {
	printf(" getM Id    ---Show a record by id from the first table\n");
	printf(" getS id1 id2  ---show subrecord by foreign key\n");
	printf(" delM id ---delete from first table \n");
	printf(" delS id1 id2 ---delete subrecord from second table\n");
	printf(" updateS id1 id2 text ---update subrecord from second table with indexes\n");
	printf(" insertM id text ---create in first\n");
	printf(" insertS id id2 text ---crreate in second ( subrecord )\n");
	printf(" ---------------------------------------------------------\n");
}
//Create or open existing files
void openOrCreateFile() {
	//init files
	if ((Sfl = fopen(file_name_fl, _modeTemplate_rb)) == NULL)
		if ((Sfl = fopen(file_name_fl, _modeTemplate_wb)) == NULL) {
			exit(0);
		}

	if ((Sind = fopen(file_name_ind, _modeTemplate_rb)) == NULL)
		if ((Sind = fopen(file_name_ind, _modeTemplate_wb)) == NULL) {
			exit(0);
		}

	if ((SP = fopen(file_name_flSp, _modeTemplate_rb)) == NULL)
		if ((SP = fopen(file_name_flSp, _modeTemplate_wb)) == NULL) {
			exit(0);
		}
}
//terminal
void waitingForResponseTerminal() {
	//menu sector

	char text[50];
	char comm[50];

	int arg1 = 0;
	int arg2 = 0;
	char arg3[50];
	while (1) {
		gets_s(text);
		sscanf(text, "%s", comm);

		//parsing
		if (strcmp(comm, comm_getM) == 0) {
			if (sscanf(text, "%s %d", comm, &arg1) == 2) {
				if (getM(arg1, text) != -1) printf(" %d: %s \n", arg1, text);
				else printf(" error\n");
			}
			else printf(" error argument\n");
		}
		else if (strcmp(comm, comm_getS) == 0) {
			if (sscanf(text, "%s %d %d", comm, &arg1, &arg2) == 3) {
				if (getS(arg1, arg2, text) != -1) printf(" %d->%d: %s \n", arg1, arg2, text);
				else printf(" error\n");
			}
			else printf(" error argument\n");
		}

		else if (strcmp(comm, comm_delM) == 0) {
			if (sscanf(text, "%s %d", comm, &arg1) == 2) {
				if (delM(arg1) != -1) printf(" ok\n");
				else printf(" error\n");
			}
			else printf(" error argument\n");
		}

		else if (strcmp(comm, comm_delS) == 0) {
			if (sscanf(text, "%s %d %d", comm, &arg1, &arg2) == 3) {
				if (delS(arg1, arg2) != -1) printf(" ok\n");
				else printf(" error\n");
			}
			else printf(" error argument\n");
		}

		else if (strcmp(comm, comm_updateM) == 0) {
			if (sscanf(text, "%s %d %s", comm, &arg1, arg3) == 3) {
				if (updateM(arg1, arg3) != -1) printf(" ok\n");
				else printf(" error\n");
			}
			else printf(" error argument\n");
		}

		else if (strcmp(comm, comm_updateS) == 0) {
			if (sscanf(text, "%s %d %d %s", comm, &arg1, &arg2, arg3) == 4) {
				if (updateS(arg1, arg2, arg3) != -1) printf(" ok\n");
				else printf(" error\n");
			}
			else printf(" error argument\n");
		}

		else if (strcmp(comm, comm_insertM) == 0) {
			if (sscanf(text, "%s %d %s", comm, &arg1, arg3) == 3) {
				if (insertM(arg1, arg3) != -1) printf(" ok\n");
				else printf(" error\n");
			}
			else printf(" error argument\n");
		}

		else if (strcmp(comm, comm_insertS) == 0) {
			if (sscanf(text, "%s %d %d %s", comm, &arg1, &arg2, arg3) == 4) {
				if (insertS(arg1, arg2, arg3) != -1) printf(" ok\n");
				else printf(" error\n");
			}
			else printf(" error argument\n");
		}
		else if (strcmp(comm, "help") == 0) {
			printf("bulo len pisat\n");
		}
		else {
			printf("command not found\n");
			//comm not found;
		}
	}
}
int main() {
	showInterface();
	openOrCreateFile();
	waitingForResponseTerminal();

	system("pause");
}





struct indexM {
	short id;
	short isA;
	short address;
};
