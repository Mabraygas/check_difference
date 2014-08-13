#include <stdio.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <string.h>
#include "uks_curl.h"
#include "JsonBox/include/JsonBox.h"

#define MAX_PRIM 999983
#define MAXL 1000
#define check_quantity 1000 //检查搜索词数量

const double epsilon = 1e-5;  //比较精度
using namespace std;

struct Video {
    Video () {}
    Video(long long _docID, double _score, double _matchDegree, int _score15Days, double _videoRank, double _videoScore)
        : docID(_docID), score(_score), matchDegree(_matchDegree), score15Days(_score15Days), videoRank(_videoRank), videoScore(_videoScore) {}
    long long docID;
    int score15Days;
    double matchDegree, score, videoRank, videoScore;
};

long long stoi(string str) {
    long long sum = 0;
    for(int i = 0; i < str.length(); i ++ )
        sum = sum * 10l + str[i] - '0';
    return sum;
}

int main() {
    FILE* fp, * fout;
    char buf[MAXL];
    if((fp = fopen("words.ini", "r")) == NULL) {
        printf("INIT FILE ERROR!!\n");
        return 0;
    }
    if((fout = fopen("result.out", "a+")) == NULL) {
        printf("OUT FILE OPEN ERROR!!\n");
        return 0;
    }
    int error_flag[6];
    string docID;
    int score15Days;
    double matchDegree, score, videoRank, videoScore;
    const string attribute[6] = {"docID", "score", "matchDegree", "score15Days", "videoRank", "videoScore"};
    const string TOTAL = "total";
    const string item = "items";
    int file_cnt = 0;
    int Loop = 0;
    int different_count = 0;
    while(fgets(buf, MAXL, fp) != NULL) {
        if(file_cnt++ > check_quantity)
            break;
        cout<<"now: "<<file_cnt<<" ";
        if(buf[0] == '#')
            continue;
        Loop = 0;
        different_count = 0;
    LOOP:
        CHttpClient CHC;
        string response = "";
        vector<vector<Video> > IS(MAX_PRIM + 10);
        JsonBox::Value value_of_json;
        if(Loop == 0) {
            while(buf[strlen(buf) - 1] == '\n' || buf[strlen(buf) - 1] == ' ' || buf[strlen(buf) - 1] == '\r')
                buf[strlen(buf) - 1] = '\0';
            fwrite(buf, sizeof(char), strlen(buf), fout);
            fputc('\n', fout);
        }
        string url = "http://10.100.8.107/search_video/q_?cmd=resort&q=" + string(buf);
        string url_compare = "http://10.100.8.106/search_video/q_?cmd=resort&q=" + string(buf);
       
        int ret = CHC.Get(url, response);
        int cError_cnt = 0;
        while(ret) {
            if(cError_cnt == 0)
                printf("Connection Error On %s!\n",buf);
            if(++cError_cnt > 50) {
                fputc('\t', fout);
                fputs("Connect Error\n", fout);
                break;
            }
        }
        if(cError_cnt > 50)
            continue;
        value_of_json.loadFromString(response);
        int total;
        
        total = value_of_json["total"].getInt();
        cout<<total<<" results for \""<<buf<<"\"\n";
        
        for(int i = 0; i < total; i ++ ) {
            char vid[10];
            snprintf(vid, sizeof(vid), "%d", i);
            const string Vid = string(vid);
            docID = value_of_json["items"][vid]["docID"].getString();
            score = value_of_json["items"][vid]["score"].getDouble();
            matchDegree = value_of_json["items"][vid]["matchDegree"].getDouble();
            score15Days = value_of_json["items"][vid]["score15Days"].getInt();
            videoRank = value_of_json["items"][vid]["videoRank"].getDouble();
            videoScore = value_of_json["items"][vid]["videoScore"].getDouble();
            
            long long  DID = stoi(docID);
            IS[DID % MAX_PRIM].push_back(Video(DID, score, matchDegree, score15Days, videoRank, videoScore));
        }
        
        response = "";
        ret = CHC.Get(url_compare, response);
        cError_cnt = 0;
        while(ret) {
            if(cError_cnt == 0)
                printf("Connection Error On %s!\n",buf);
            if(++cError_cnt > 50)
                break;
        }
        value_of_json.loadFromString(response); 
        //total = value_of_json["total"].getInt();
        for(int i = 0; i < total; i ++ ) {
            char vid[10];
            snprintf(vid, sizeof(vid), "%d", i);
            const string Vid = string(vid);
            docID = value_of_json["items"][vid]["docID"].getString();
            score = value_of_json["items"][vid]["score"].getDouble();
            matchDegree = value_of_json["items"][vid]["matchDegree"].getDouble();
            score15Days = value_of_json["items"][vid]["score15Days"].getInt();
            videoRank = value_of_json["items"][vid]["videoRank"].getDouble();
            videoScore = value_of_json["items"][vid]["videoScore"].getDouble();
            
            long long  DID = stoi(docID);

            int find = 0; 
            vector<Video> :: iterator it = IS[DID % MAX_PRIM].begin();
            for(; it != IS[DID % MAX_PRIM].end(); it ++ ) {
                if(it->docID == DID) {
                    find = 1;
                    if(fabs(it->score - score) > epsilon || fabs(it->matchDegree - matchDegree) > epsilon
                        || it->score15Days != score15Days || fabs(it->videoRank - videoRank) > epsilon
                        || fabs(it->videoScore - videoScore) > epsilon)
                    {
                        find = 0;
                        //cout<<buf<<" "<<it->docID<<" "<<it->score<<" "<<score<"  ";
                    }
                    if(find == 1)
                        break;
                }
            }
            if(find == 0 && DID > 0) {
                if(Loop <= 5) {
                    Loop ++ ;
                    cout<<"LOOP"<<'\n';
                    goto LOOP;
                }
                else {
                    cout<<DID<<'\n';
                    different_count ++ ;
                    fputc('\t',fout);
                    fwrite(docID.c_str(), sizeof(char), docID.length(), fout);
                    fputc('\n', fout);
                }
            }
        }
        if(different_count == 0) {
            fputc('\t',fout);
            fputs("No Any Difference", fout);
        }
        fputc('\n', fout);
    }
    fclose(fout);
    fout = NULL;
    fclose(fp);
    fp = NULL;
    return 0;
}
