#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>
#include "cJSON.h"

char *do_web_request(char *url);
size_t static write_callback_func(void *buffer,size_t size,size_t nmemb,void *userp);
int first_time=1;

int main()
{
	char *myplaces[] = {"daliang","guangzhou","hongkong","shanghai","xiamen","beijing","taipei-city","lhasa","chongqing","mohe","sanya"};
	char *myplaces_zh[] = {"大良","广州","香港","上海","厦门","北京","臺北","拉萨","重庆","漠河","三亚"};
	int total_places=sizeof myplaces/sizeof *myplaces;
	int i=0;
	int columns=3;
	char placestxt[500];
	char ccc[3];
	while(i<total_places){
		sprintf(ccc, "%d", i+1);
		if(i==0)strcpy(placestxt,ccc);else strcat(placestxt,ccc);
		strcat(placestxt,".");
		strcat(placestxt,"[");
		strcat(placestxt,myplaces_zh[i]);
		strcat(placestxt,"]");
		strcat(placestxt,myplaces[i]);
		if(strlen(myplaces[i])+strlen(myplaces_zh[i])*2/3+strlen(ccc)+3<16){
			strcat(placestxt,"\t\t\t");
		}else{
			strcat(placestxt,"\t\t");
		}
		if(i%columns==columns-1){
			strcat(placestxt,"\n");
		}
		i++;
	}
	if(first_time!=1)printf("\n\n\n");
	printf("cWeather v0.1 - github.com/caiguanhao/cWeather\n");
	printf("输入以下任一地名（中文或英文）或其编号查看即时天气信息：\n\n%s\n\n获取信息需要等约5秒时间。输入exit或按CTRL+C退出。默认为1/大良/daliang：\n\n",placestxt);
	char input[100];
	char dplace[100];
	strcpy(input,"http://www.caiguanhao.com/custom-homepage/weather/");
	char *inp=gets(dplace);
	int dpx=(int)strtod(dplace,NULL);
	int selected_place_index=-1;
	if(!*inp){
		strcpy(dplace,"daliang");
	}else if(strcmp(dplace,"exit")==0){
		return 0;
	}else if(dpx>0&&dpx<=total_places){
		selected_place_index=dpx-1;
		strcpy(dplace,myplaces[selected_place_index]);
	}
	if(selected_place_index==-1){
		i=0;
		while(i<total_places){
			if(strcmp(dplace,myplaces[i])==0){
				selected_place_index=i;
				break;
			}
			i++;
		}
		if(selected_place_index==-1){
			i=0;
			while(i<total_places){
				if(strcmp(dplace,myplaces_zh[i])==0){
					selected_place_index=i;
					strcpy(dplace,myplaces[i]);
					break;
				}
				i++;
			}
		}
	}
	if(selected_place_index==-1){
		printf("错误的输入。请重新输入。\n");
	}else{
	strcat(input,dplace);
	strcat(input,".json");
    char *url = input;
    char *content = NULL;
	char *tim,*tem,*con,*hum,*pre,*vis,*wid,*wsp,*tco,*ttl,*tth,*sur,*sus,*lnk;
	cJSON *json;
	
	content = do_web_request(url);

	json=cJSON_Parse(content);
	
	tim = cJSON_GetObjectItem(json,"time")->valuestring;
	tem = cJSON_GetObjectItem(json,"temperature")->valuestring;
	con = cJSON_GetObjectItem(json,"condition")->valuestring;
	hum = cJSON_GetObjectItem(json,"humidity")->valuestring;
	pre = cJSON_GetObjectItem(json,"pressure")->valuestring;
	if(cJSON_GetObjectItem(json,"visibility")){
		vis = cJSON_GetObjectItem(json,"visibility")->valuestring;
	}else{
		vis = "n/a";
	}
	if(cJSON_GetObjectItem(cJSON_GetObjectItem(json,"wind"),"direction")){
		wid = cJSON_GetObjectItem(cJSON_GetObjectItem(json,"wind"),"direction")->valuestring;
	}else{
		wid = "n/a";
	}
	wsp = cJSON_GetObjectItem(cJSON_GetObjectItem(json,"wind"),"speed")->valuestring;
	tco = cJSON_GetObjectItem(cJSON_GetObjectItem(json,"tomorrow"),"condition")->valuestring;
	ttl = cJSON_GetObjectItem(cJSON_GetObjectItem(json,"tomorrow"),"low")->valuestring;
	tth = cJSON_GetObjectItem(cJSON_GetObjectItem(json,"tomorrow"),"high")->valuestring;
	sur = cJSON_GetObjectItem(json,"sunrise")->valuestring;
	sus = cJSON_GetObjectItem(json,"sunset")->valuestring;
	lnk = cJSON_GetObjectItem(json,"link")->valuestring;
	
	char hrl[500];
	strcpy(hrl,"-");
	for(i=1;i<strlen(lnk)+4;i++){
		strcat(hrl,"-");
	}
	printf("\n|   Real-Time Weather Condition of %s - %s的即时天气 - %s\n%s\n|   %s，%s，湿度%s，气压%s，能见度%s\n|   吹%s风，风速%s，%s日出，%s日落，预计明天%s，%s至%s\n|\n|   From %s, which parses data from\n|   %s\n\n",dplace,myplaces_zh[selected_place_index],tim,hrl,tem,con,hum,pre,vis,wid,wsp,tco,ttl,tth,sur,sus,url,lnk);
	
	cJSON_Delete(json);
	
	}
	first_time=0;
	return main();
}

char *do_web_request(char *url)
{
	CURL *curl_handle = NULL;
	char *response = NULL;
	curl_handle = curl_easy_init();
	curl_easy_setopt(curl_handle, CURLOPT_URL, url);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_callback_func);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &response);
	//curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(curl_handle, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
	//struct curl_slist *slist=NULL;
	//slist = curl_slist_append(slist, "Expect:");
	//curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, slist);
	curl_easy_perform(curl_handle);
	curl_easy_cleanup(curl_handle);
	return response;
}

size_t static write_callback_func(void *buffer,size_t size,size_t nmemb,void *userp)
{
    char **response_ptr =  (char**)userp;

    /* assuming the response is a string */
    *response_ptr = strndup(buffer, (size_t)(size *nmemb));
	return 0;
}
