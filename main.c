// SC対して近傍をつくる

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

int read_exams(FILE *fp, int exam[][500]);
int read_rooms(FILE *fp, int room[][2]);
int read_periods(FILE *fp, int period[][2],int day[]);
void read_PeriodHardConstraints(FILE *fp,int PHC[][3]);
void read_RoomHardConstraints(FILE *fp, int RHC[]);
void read_InstitutionalWeightings(FILE *fp, int IW[]);
//同じ生徒が同じ時間帯に二つの試験を受けていないか
int count_HC_0(int result[][2], int n_exams, int n_periods, int exam[][500]);
//部屋のキャパ
int count_HC_1(int result[][2], int n_exams, int n_periods, int n_rooms, int exam[][500], int room[][2]);
//定刻を超えない
int count_HC_2(int result[][2], int n_exams, int period[][2], int exam[][500]);
//PeriodHArdConstraint
int count_HC_3(int result[][2], int PHC[][3]);
//RoomHardConstraint
int count_HC_4(int result[][2], int RHC[], int n_exam);
//未割当の試験を探す
int count_HC_5(int result[][2], int n_exam);
//HCを計算する
int count_HC(int result[][2], int n_exams, int n_periods, int n_rooms, int exam[][500], int period[][2], int room[][2], int PHC[][3], int RHC[]);
// Two Exams in a Row(Soft Constraints)
int count_SC_0(int result[][2], int exam[][500], int n_exam, int n_period, int IW[], int day[]);
// Two Exams in a Day
int count_SC_1(int result[][2], int exam[][500], int n_exam, int n_period, int IW[], int day[]);
// Period Spread
int count_SC_2(int result[][2], int exam[][500], int n_exam, int n_period, int IW[], int day[]);
// Non Mixed Duration
int count_SC_3(int result[][2], int exam[][500], int n_exam, int n_period, int n_room, int IW[]);
// Larger Exams towards the beginning of the examination session
int count_SC_4(int result[][2], int exam[][500], int IW[], int largest_exam[], int n_period);
int count_SC_5(int result[][2], int period[][2], int n_exam);
int count_SC_6(int result[][2], int room[][2], int n_exam);
int count_SC(int result[][2], int exam[][500], int n_exam, int n_period, int IW[], int day[], int largest_exam[], int room[][2], int n_room, int period[][2], int flag);
int Exam_swap(int result[][2], int n_exams, int n_periods, int n_rooms, int exam[][500], int period[][2], int room[][2], int PHC[][3], int RHC[], int HC, int SC, int IW[], int day[], int largest_exam[]);
int Period_change(int result[][2], int n_exams, int n_periods, int n_rooms, int exam[][500], int period[][2], int room[][2], int PHC[][3], int RHC[], int HC, int SC, int IW[], int day[], int largest_exam[]);
int Room_change(int result[][2], int n_exams, int n_periods, int n_rooms, int exam[][500], int period[][2], int room[][2], int PHC[][3], int RHC[], int HC, int SC, int IW[], int day[], int largest_exam[]);
int Period_swap(int result[][2], int n_exams, int n_periods, int n_rooms, int exam[][500], int period[][2], int room[][2], int PHC[][3], int RHC[], int HC);
int Room_swap(int result[][2], int n_exams, int n_periods, int n_rooms, int exam[][500], int period[][2], int room[][2], int PHC[][3], int RHC[], int HC);
int Period_room_change(int result[][2], int n_exams, int n_periods, int n_rooms, int exam[][500], int period[][2], int room[][2], int PHC[][3], int RHC[], int HC, int SC, int IW[], int day[], int largest_exam[]);
int Exam_swap_origin(int result[][2], int n_exams, int n_periods, int n_rooms, int exam[][500], int period[][2], int room[][2], int PHC[][3], int RHC[], int HC);
void neighbor(int result[][2], int n_exams, int n_periods, int n_rooms, int exam[][500], int period[][2], int room[][2], int PHC[][3], int RHC[], int HC, int SC, int IW[], int day[], int largest_exam[]);
void swap_num(int *a, int *b);
void swap_result(int result[][2], int x, int y);
int linear_search(int* array, size_t size, int value);

int main(void){
    int exam[1000][500];
    int room[100][2];
    int period[100][2];
    int PHC[100][3]; //PeriodHardConstraints [0]:A [1]:EXAM_COINCIDENCE [2]:Bみたいになってる
    int RHC[5]; //RoomHardConstraints
    int IW[7]; //InstitutionalWeightnings
    int day[100];
    int n_exams;
    int n_rooms;
    int n_periods;
    int i, j;
    int HC = 0;//Number of violation of HC
    clock_t start,end;
    srand(time(NULL));

    start = clock();
    //データ読み取り
    FILE *fp;
    fp = fopen("C:\\Users\\gakuo\\OneDrive - Tokyo University of Agriculture and Technology\\study\\B2_kouki\\SAIL\\program\\debug_SC\\data_genpon.txt","r");
    if(fp == NULL){
        printf("file data_genpon not found.");
        return -1;
    }
    n_exams = read_exams(fp, exam);
    n_periods = read_periods(fp,period,day);
    n_rooms = read_rooms(fp,room);
    read_PeriodHardConstraints(fp,PHC);
    read_RoomHardConstraints(fp,RHC);
    read_InstitutionalWeightings(fp, IW);

    fclose(fp);

    /*** データの読み込み ***/
    int result[n_exams][2];
    char *fname = "C:\\Users\\gakuo\\OneDrive - Tokyo University of Agriculture and Technology\\study\\B2_kouki\\SAIL\\program\\debug_SC\\result.csv";
    int ret;
    i = 0;

    fp = fopen( fname, "r" );
    if( fp == NULL ){
        printf( "%s not found.\n", fname );
        return -1;
    }

    while( (ret=fscanf(fp, "%d,%d", &result[i][0], &result[i][1])) != EOF){
        // printf("%d %d \n", result[i][0], result[i][1]);
        i++;
    }

    fclose( fp );

    // largest examを生成
    int num_people[n_exams];
    for (i = 0; i < n_exams; i++){
        for (j = 0; exam[i][j]!=-1; j++);
        num_people[i] = j;
    }

    int largest_exam[IW[4]];
    int buf;
    for ( i = 0; i < IW[4]; i++){
        int buf = num_people[0];
        int index = 0;
        for (j = 1; j < n_exams; j++){
            if(num_people[j] > buf){
                buf = num_people[j];
                index = j;
            }
        }
        largest_exam[i] = index;
        num_people[index] = -1;
    }
    
    fp = fopen("SC.csv", "w");
    fprintf(fp,"i,HC,HC0,HC1,HC2,HC3,HC4,HC5,SC0,SC1,SC2,SC3,SC4,SC5,SC6\n");
    HC = count_HC(result, n_exams, n_periods, n_rooms, exam, period, room, PHC, RHC);
    int HC0 = count_HC_0(result, n_exams, n_periods, exam);
    int HC1 = count_HC_1(result, n_exams, n_periods, n_rooms, exam, room);
    int HC2 = count_HC_2(result, n_exams, period, exam);
    int HC3 = count_HC_3(result, PHC);
    int HC4 = count_HC_4(result, RHC, n_exams);
    int HC5 = count_HC_5(result, n_exams);
    int SC = count_SC(result, exam, n_exams, n_periods, IW, day, largest_exam, room, n_rooms, period, 0);
    int SC0 = count_SC_0(result, exam, n_exams, n_periods, IW, day);
    int SC1 = count_SC_1(result, exam, n_exams, n_periods, IW, day);
    int SC2 = count_SC_2(result, exam, n_exams, n_periods, IW, day);
    int SC3 = count_SC_3(result, exam, n_exams, n_periods, n_rooms, IW);
    int SC4 = count_SC_4(result, exam, IW , largest_exam, n_periods);
    int SC5 = count_SC_5(result, period, n_exams);
    int SC6 = count_SC_6(result, room, n_exams);
    for (i = 0;i < 1000; i++)
    {
        /* code */
        neighbor(result, n_exams, n_periods, n_rooms, exam, period, room, PHC, RHC, HC,SC,IW,day,largest_exam);
        HC = count_HC(result, n_exams, n_periods, n_rooms, exam, period, room, PHC, RHC);
        HC0 = count_HC_0(result, n_exams, n_periods, exam);
        HC1 = count_HC_1(result, n_exams, n_periods, n_rooms, exam, room);
        HC2 = count_HC_2(result, n_exams, period, exam);
        HC3 = count_HC_3(result, PHC);
        HC4 = count_HC_4(result, RHC, n_exams);
        HC5 = count_HC_5(result, n_exams);
        SC = count_SC(result, exam, n_exams, n_periods, IW, day, largest_exam, room, n_rooms, period, 0);
        SC0 = count_SC_0(result, exam, n_exams, n_periods, IW, day);
        SC1 = count_SC_1(result, exam, n_exams, n_periods, IW, day);
        SC2 = count_SC_2(result, exam, n_exams, n_periods, IW, day);
        SC3 = count_SC_3(result, exam, n_exams, n_periods, n_rooms, IW);
        SC4 = count_SC_4(result, exam, IW , largest_exam, n_periods);
        SC5 = count_SC_5(result, period, n_exams);
        SC6 = count_SC_6(result, room, n_exams);
        printf("i = %d\n",i);
        fprintf(fp,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,\n",i,HC,HC0,HC1,HC2,HC3,HC4,HC5,SC,SC0,SC1,SC2,SC3,SC4,SC5,SC6);
        if(i%10000==0){
            fp = fopen("result.csv","w");
            for (int j = 0; j < n_exams; j++)
            {
                fprintf(fp,"%d,%d\n",result[j][0],result[j][1]);
            }
            fclose(fp);
        }
    }

    fp = fopen("result_SC.csv","w");
    for (int j = 0; j < n_exams; j++)
    {
        fprintf(fp,"%d,%d\n",result[j][0],result[j][1]);
    }
    fclose(fp);

    return 0;
}

int read_exams(FILE *fp, int exam[][500]){
    int n_exam;
    char buf[10000];
    char buf_n_exam[10];
    int i = 0;
    int j = 0;
    int k = 0;
    int l = 0;

    //試験の個数の読み取り
    fgets(buf,sizeof(buf),fp);
    while(buf[i]!='\0'){
        if(isdigit(buf[i])){
            // printf("%c\n",buf[i]);
            buf_n_exam[j]=buf[i];
            j++;
        }
        i++;
    }
    buf_n_exam[j]='\0';
    n_exam = atoi(buf_n_exam);

    // //データの読み取り
    while(l<n_exam){
        i=0;
        j=0;
        k=0;
        fgets(buf,sizeof(buf),fp);
        // printf("%s",buf);
        while(1){
            while(isdigit(buf[i])){
                // printf("%c\n",buf[i]);
                buf_n_exam[j]=buf[i];
                j++;
                i++;
            }
            buf_n_exam[j]='\0';
            exam[l][k]=atoi(buf_n_exam);
            // printf("%d, ",exam[l][k]);
            if(buf[i]=='\n') break;
            j = 0;
            i += 2;
            k++;
        }
        k++;
        exam[l][k] = -1;
        l++;
    }
    exam[l][0] = -1;
    return n_exam;
}
int read_rooms(FILE *fp, int room[][2]){
    int n_room;
    char buf[2048];
    char buf_n_rooms[10];
    int i = 0;
    int j = 0;
    int k = 0;
    int l = 0;

    //試験の個数の読み取り
    fgets(buf,sizeof(buf),fp);
    while(buf[i]!='\0'){
        if(isdigit(buf[i])){
            // printf("%c\n",buf[i]);
            buf_n_rooms[j]=buf[i];
            j++;
        }
        i++;
    }
    buf_n_rooms[j]='\0';
    n_room = atoi(buf_n_rooms);
    // printf("%d\n",n_rooms);

    while(l < n_room){
        fgets(buf,sizeof(buf),fp);
        i = 0;
        j = 0;
        k = 0;
        while(1){
            while(isdigit(buf[i])){
                buf_n_rooms[j] = buf[i];
                i++;
                j++;
                
            }
            buf_n_rooms[j] = '\0';
            room[l][k] = atoi(buf_n_rooms);
            // printf("%d, ",room[l][k]); 
            if(buf[i]=='\n') break;
            j = 0;
            i += 2;
            k++;
        }
        // printf("\n");
        l++;
    }
    room[l][0]=-1;
    return n_room;
}
int read_periods(FILE *fp,int period[][2], int day[]){
    int n_period;
    char buf[2048];
    char buf_n_periods[10];
    char buf_day[12];
    int __day = 0;//探索中の日付を格納
    int flag = 0;//日付探索が最初かどうか（最初：0、それ以降：1）
    int i = 0;
    int j = 0;
    int k = 0;
    int l = 0;
    int m = 0;// 月日の読み取り

    //時間帯の個数の読み取り
    fgets(buf,sizeof(buf),fp);
    while(buf[i]!='\0'){
        if(isdigit(buf[i])){
            // printf("%c\n",buf[i]);
            buf_n_periods[j]=buf[i];
            j++;
        }
        i++;
    }
    buf_n_periods[j]='\0';
    n_period = atoi(buf_n_periods);
    // printf("%d\n",n_periods);

    while(l < n_period){
        fgets(buf,sizeof(buf),fp);
        i = 22;
        j = 0;
        k = 0;
        // 月日の読み取り
        buf_day[0] = buf[6];
        buf_day[1] = buf[7];
        buf_day[2] = buf[8];
        buf_day[3] = buf[9];
        buf_day[4] = buf[3];
        buf_day[5] = buf[4];
        buf_day[6] = buf[0];
        buf_day[7] = buf[1];
        buf_day[8] = '\0';
        
        // if(flag==0){
        //     __day=atoi(buf_day);
        //     day[m]=1;
        //     flag=1;
        // }

        // else if (__day!=atoi(buf_day)){
        //     __day=atoi(buf_day);
        //     m++;
        //     day[m]=1;
        // }else{
        //     day[m]++;
        // }
        day[l] = atoi(buf_day);
        while(1){
            //時間、ペナルティの読み取り
            while(isdigit(buf[i])){
                buf_n_periods[j] = buf[i];
                i++;
                j++;
            }
            buf_n_periods[j] = '\0';
            period[l][k] = atoi(buf_n_periods);
            // printf("%d, ",period[l][k]); 
            if(buf[i]=='\n') break;
            j = 0;
            i += 2;
            k++;
        }
        // printf("\n");
        l++;
    }
    period[l][0]=-1;
    day[l]=-1;
    return n_period;
}
void read_PeriodHardConstraints(FILE *fp, int PHC[][3]){
    char buf[1024];
    char buf_n[30];
    int i = 0;
    int j = 0;
    int k = 0;
    fgets(buf,sizeof(buf),fp);
    fgets(buf,sizeof(buf),fp);
    while(strcmp(buf,"[RoomHardConstraints]\n")){
        //最初の数字
        // printf("%s",buf);
        while(isdigit(buf[i])){
            buf_n[j] = buf[i];
            j++;
            i++;
        }
        buf_n[j] = '\0';
        PHC[k][0] = atoi(buf_n);
        j=0;
        // printf("%d\n",PHC[k][0]);
        //条件
        while(!isdigit(buf[i])){
            buf_n[j] = buf[i];
            j++;
            i++;
        }
        buf_n[j] = '\0';
        j=0;
        if(!strcmp(buf_n,", EXAM_COINCIDENCE, "))PHC[k][1]=0;
        if(!strcmp(buf_n,", EXCLUSION, "))PHC[k][1]=1;
        if(!strcmp(buf_n,", AFTER, "))PHC[k][1]=2;
        // printf("%d\n",PHC[k][1]);
        //二つ目の数字
        while(isdigit(buf[i])){
            buf_n[j] = buf[i];
            j++;
            i++;
        }
        buf_n[j] = '\0';
        j = 0;
        PHC[k][2] = atoi(buf_n);
        // printf("%d\n",PHC[k][2]);
        fgets(buf,sizeof(buf),fp);
        k++;
        i = 0;
    }
    PHC[k][0]=-1;
}
void read_RoomHardConstraints(FILE *fp, int RHC[]){
    char buf[1024];
    char buf_n[30];
    int i = 0;
    int j = 0;
    int k = 0;
    fgets(buf,sizeof(buf),fp);
    while(strcmp(buf,"[InstitutionalWeightings]\n")){
        //最初の数字
        // printf("%s",buf);
        while(isdigit(buf[i])){
            buf_n[j] = buf[i];
            j++;
            i++;
        }
        buf_n[j] = '\0';
        RHC[k] = atoi(buf_n);
        j=0;
        fgets(buf,sizeof(buf),fp);
        k++;
        i = 0;
    }
    RHC[k]=-1;
}
void read_InstitutionalWeightings(FILE *fp, int IW[]){
    char buf[100];
    char buf_n[10];
    int i = 0;
    int j = 0;
    int l  = 0;
    //NONMIXEDDURATIONSまで
    for (i = 0; i < 4; i++){
        fgets(buf,sizeof(buf),fp);
        while(!isdigit(buf[l]))l++;
        while(isdigit(buf[l])){
            buf_n[j] = buf[l];
            j++;
            l++;
        }
        buf_n[j] = '\0';
        IW[i] = atoi(buf_n);
        l = 0;
        j = 0;
    }

    //FRONTLOAD
    fgets(buf,sizeof(buf),fp);
    while(!isdigit(buf[l]))l++;
    for(int m = 0; m < 3; m++){
        while(isdigit(buf[l])){
            buf_n[j] = buf[l];
            j++;
            l++;
        }
        buf_n[j] = '\0';
        IW[i] = atoi(buf_n);
        j=0;
        l++;
        i++;
    }
}
int count_HC_0(int result[][2], int n_exams, int n_periods, int exam[][500]){
    int i = 0;//時間割アクセス用
    int j = 0;//配列アクセス用
    int k = 0;//試験アクセス用
    int l = 0;//stuアクセス用
    int stu[10000];//同じ時間帯に受ける生徒の学籍番号を格納する
    int HC_num = 0;//ハード制約違反の点数
    for(i = 0;i < n_periods;i++){
        l = 0;
        for(j = 0;j < n_exams;j++){
            if(result[j][0]==i){//i番目の時間帯に割り当てられていたら
                int k = 1;//試験アクセス用
                while(exam[j][k]!=-1){
                    stu[l]=exam[j][k];
                    k++;
                    l++;
                }
            }
        }
        stu[l] = -1;
        l = 0;
        while(stu[l]!=-1){
            for(int m = l+1;stu[m]!=-1;m++){
                if(stu[l]==stu[m]) HC_num++;
            }
            l++;
        }
    }
    return HC_num;
}
int count_HC_1(int result[][2], int n_exams, int n_periods, int n_rooms, int exam[][500], int room[][2]){
    int HC_num = 0;//ハード制約違反の点数
    int num = 0;//部屋にはいる人数
    for (int i = 0; i < n_periods; i++){
        for(int j = 0; j < n_rooms; j++){
            int num = 0;
            for(int k = 0; k < n_exams; k++){
                //インデックスがkのテストがi時間目に教室jを使う場合、numにその試験の人数を足す
                if(result[k][0]==i && result[k][1]==j){
                    for(int l = 1; exam[k][l]!=-1; l++) num++;
                }
            }
            if(num > room[j][0])HC_num += num-room[j][0];
        }
    }
    return HC_num;
}
int count_HC_2(int result[][2], int n_exams, int period[][2], int exam[][500]){
    int HC_num = 0;
    for (int i = 0; i < n_exams; i++){
        if(result[i][0]==-1)continue;
        else if(period[result[i][0]][0] < exam[i][0])HC_num++;
    }
    return HC_num;
}
int count_HC_3(int result[][2], int PHC[][3]){
    int HC_num = 0;
    for(int i = 0; PHC[i][0]!=-1; i++){
        if(result[PHC[i][0]][0]==-1 || result[PHC[i][2]][0]==-1)continue;
        switch(PHC[i][1]){
            case 0:// EXAM_COINCIDENCE
                if(result[PHC[i][0]][0]!=result[PHC[i][2]][0])HC_num++;
                break;
            case 1:// EXCLUSION
                if(result[PHC[i][0]][0]==result[PHC[i][2]][0])HC_num++;
                break;
            case 2://AFTER
                if(result[PHC[i][0]][0] > result[PHC[i][2]][0])HC_num++;
                break;
        }
    }
    return HC_num;
}
int count_HC_4(int result[][2], int RHC[], int n_exam){
    int HC_num = 0;
    for (int i = 0; RHC[i]!=-1; i++){
        for (int j = 0; j < n_exam; j++){
            if(result[RHC[i]][0]==-1 || result[j][0]==-1)continue;
            if(result[RHC[i]][0]==result[j][0] && result[RHC[i]][1]==result[j][1] && j!=RHC[i]) HC_num++;
        }
    }
    return HC_num;
}
int count_HC_5(int result[][2], int n_exam){
    int HC_num = 0;
    for(int i = 0; i < n_exam; i++){
        if(result[i][0]==-1) HC_num++;
    }
    return HC_num;
}
int count_HC(int result[][2], int n_exams, int n_periods, int n_rooms, int exam[][500], int period[][2], int room[][2], int PHC[][3], int RHC[]){
    int sum_HC = 0;
    sum_HC += count_HC_0(result, n_exams, n_periods, exam);
    sum_HC += count_HC_1(result, n_exams, n_periods, n_rooms, exam, room);
    sum_HC += count_HC_2(result, n_exams, period, exam);
    sum_HC += count_HC_3(result, PHC);
    sum_HC += count_HC_4(result, RHC, n_exams);
    sum_HC += count_HC_5(result, n_exams);
    return sum_HC;
}

int count_SC_0(int result[][2], int exam[][500], int n_exam, int n_period, int IW[], int day[]){
    int stu_i[5000];// i時間目に受ける生徒
    int stu_i1[5000];// i+1時間目に受ける生徒
    int counter = 0;// k,k_1用のカウンタ
    int l = 1;// exam用のカウンタ
    int num_stu = 0;
    int SC_sum = 0;



    for (int i = 0; i < n_period; i++)
    {
        /* code */
        // -1で初期化
        for(int a = 0;a < 5000; a++){
            stu_i[a]=-1;
            stu_i1[a]=-1;
        }
        // i+1時間目が次の日ならcontinue
        if(day[i]!=day[i+1])continue;
        
        counter = 0;

        // i時間目に受ける生徒の学籍番号を配列kに格納 
        for(int j = 0; j < n_exam; j++){
            if(result[j][0]==i){
                l = 1;
                while(exam[j][l]!=-1){
                    if(!(linear_search(stu_i,5000,exam[j][l])))stu_i[counter++] = exam[j][l];
                    l++;
                }
            }
        }

        counter = 0;

        // i+1時間目に受ける生徒の学籍番号を配列kに格納 
        for(int j = 0; j < n_exam; j++){
            if(result[j][0]==i+1){
                l = 1;
                while(exam[j][l]!=-1){
                    if(!(linear_search(stu_i1,5000,exam[j][l])))stu_i1[counter++] = exam[j][l];
                    l++;
                }
            }
        }

        // 一致する生徒の数を計算
        counter = 0;
        while(stu_i[counter]!=-1){
            if(stu_i[counter]!=-1 && linear_search(stu_i1,5000,stu_i[counter++]))num_stu++;
        }
    }
    return num_stu*IW[0];
}
int count_SC_1(int result[][2], int exam[][500], int n_exam, int n_period, int IW[], int day[]){
    static int stu[20][5000];// stu[時間帯のインデックス][生徒の一時的なインデックス]
    int num_stu = 0;
    int count_day = 0;// 時間帯を参照するためのカウンタ
    int buf_day;
    int n_exam_i;// i日目の時間帯の数

    while(count_day < n_period)
    {
        // stu初期化
        for(int i = 0; i < 20; i++){
            for (int j = 0; j < 5000; j++)
            {
                stu[i][j] = -1;
            }
        }

        n_exam_i = 0;

        // count_day日目のテストに関するペナルティ
        buf_day = day[count_day];
        while(day[count_day]==buf_day){
            int j = 0;// stuの生徒のインデックス用
            //受ける生徒をstuに格納
            for (int i = 0; i < n_exam; i++){// iは試験番号
                if(result[i][0]==count_day){
                    
                    for(int l = 1; exam[i][l]!=-1; l++){
                        if(!(linear_search(stu[n_exam_i],5000,exam[i][l]))){
                            stu[n_exam_i][j++] = exam[i][l];
                        }
                    }
                }
            }
            count_day++;
            n_exam_i++;
        }
        
        // ペナルティの計算
        for (int i = 0; i < n_exam_i-2; i++){
            for (int j = i+2; j < n_exam_i; j++){
                int l = 0;
                while(stu[i][l]!=-1){
                    if(linear_search(stu[j],5000,stu[i][l]))num_stu++;
                    l++;
                }
            }
        }
    }
    return num_stu*IW[1];
}
int count_SC_2(int result[][2], int exam[][500], int n_exam, int n_period, int IW[], int day[]){
    /*** 配列の確保 ***/
    // https://tondol.hatenablog.jp/entry/20090713/1247426321
    // デバッグ時に数値を確認する方法
    // https://teratail.com/questions/283005
    int **stu, *base_stu;
    int i, j, n, m;
    int num_stu = 0;
    n = n_period, m = 5000;

    stu = malloc(sizeof(int *) * n);
    base_stu = malloc(sizeof(int) * n * m);
    for (i=0;i<n;i++) {
        stu[i] = base_stu + i * m;
    }

    // 生徒を時間帯に分ける
    for (int i = 0; i < n_period; i++){
        int counter = 0;
        for ( j = 0; j < n_exam; j++){
            if(result[j][0]==i){
                int l = 1;
                while(exam[j][l]!=-1){
                    if(!(linear_search(stu[i],5000,exam[j][l])))stu[i][counter++] = exam[j][l];
                    l++;
                }
                stu[i][counter] = -1;
            }
        }
    }
    
    // ペナルティの計算
    for (int i = 0; i < n_period; i++){// iは時間帯番号
            for(j = 0; stu[i][j]!=-1; j++){
            // ある(インデックスがjの）生徒がi時間目からi+1+IW[2]時間目までに何個の試験を受けるか計算
            for (int k = 0; k < 3 && (i+1+k) < n_period; k++){
                if(linear_search(stu[i+1+k],5000,stu[i][j]))num_stu++;
            }
        }
    }
    
    free(base_stu);
    free(stu);
    return num_stu;
}
int count_SC_3(int result[][2], int exam[][500], int n_exam, int n_period, int n_room, int IW[]){
    int exams[100];// 時間帯と部屋が一致する試験を格納
    int counter = 0;
    int num = 0;

    // examsの初期化
    for (int i = 0; i < 100; i++){
        exams[i] = -1;
    }
    
    for (int i = 0; i < n_period; i++){
        for (int j = 0; j < n_room; j++){
            for (int k = 0; k < n_exam; k++){
                // 時間帯と部屋が一致する試験を格納
                if(result[k][0]==i && result[k][1]==j && !(linear_search(exams,100,exam[k][0]))){
                    exams[counter++] = exam[k][0];
                }
            }
            for(int n = 0; exams[n] != -1; n++){
                num++;
            }
            counter = 0;
        }
    }
    return num*IW[3];
}
int count_SC_4(int result[][2], int exam[][500], int IW[], int largest_exam[], int n_period){
    int num = 0;
    for (int i = 0; i < IW[4]; i++){
        if (result[largest_exam[i]][0] >= (n_period - IW[5]) && result[largest_exam[i]][0] < n_period){
            num++;
        }
    }
    return num*IW[6];
}
int count_SC_5(int result[][2], int period[][2], int n_exam){
    int num = 0;    
    for (int i = 0; i < n_exam; i++)
    {
        num += period[result[i][0]][1];
    }
    return num;    
}
int count_SC_6(int result[][2], int room[][2], int n_exam){
    int num = 0;    
    for (int i = 0; i < n_exam; i++)
    {
        num += room[result[i][1]][1];
    }
    return num;    
}
int count_SC(int result[][2], int exam[][500], int n_exam, int n_period, int IW[], int day[], int largest_exam[], int room[][2], int n_room, int period[][2], int flag){// flag=1なら数値を出力
    int SC0, SC1, SC2, SC3, SC4, SC5, SC6, SC;
    SC0 = count_SC_0(result, exam, n_exam, n_period, IW, day);
    SC1 = count_SC_1(result, exam, n_exam, n_period, IW, day);
    SC2 = count_SC_2(result, exam, n_exam, n_period, IW, day);
    SC3 = count_SC_3(result, exam, n_exam, n_period, n_room, IW);
    SC4 = count_SC_4(result, exam, IW, largest_exam, n_period);
    SC5 = count_SC_5(result, period, n_exam);
    SC6 = count_SC_6(result, room, n_exam);
    if(flag==1){
        printf("%d, %d, %d, %d, %d, %d, %d\n",SC0,SC1,SC2,SC3,SC4,SC5,SC6);
    }
    SC = SC0 + SC1 + SC2 + SC3 + SC4 + SC5 + SC6;
    return SC;
}

int Exam_swap(int result[][2], int n_exams, int n_periods, int n_rooms, int exam[][500], int period[][2], int room[][2], int PHC[][3], int RHC[], int HC, int SC, int IW[], int day[], int largest_exam[]){
    int ret_HC, ret_SC;
    // 試験、時間帯、教室を乱数で選択
    int c_exam = rand()%n_exams;
    int c_period = rand()%n_periods;
    int c_room = rand()%n_rooms;
    // 総当たり用
    int i;
    int j;
    if(c_period == n_periods-1)i=0;
    else i = c_period+1;
    if(c_room == n_rooms-1)j=0;
    else j = c_room+1;
    // 選択した試験のもとの状態を記憶
    int tmp_period = result[c_exam][0];
    int tmp_room = result[c_exam][1];

    // 適用
    result[c_exam][0] = c_period;
    result[c_exam][1] = c_room;
    ret_HC = count_HC(result, n_exams, n_periods, n_rooms, exam, period, room, PHC, RHC);
    ret_SC = count_SC(result, exam, n_exams, n_periods, IW, day, largest_exam, room, n_rooms, period, 0);

    while(!((ret_HC < HC)||(ret_SC < SC && ret_HC <=HC))){
        // 更新
        result[c_exam][0] = i;
        result[c_exam][1] = j;
        ret_HC = count_HC(result, n_exams, n_periods, n_rooms, exam, period, room, PHC, RHC);
        ret_SC = count_SC(result, exam, n_exams, n_periods, IW, day, largest_exam, room, n_rooms, period, 0);
        if(j == c_room){//全部屋だめなら
            if(i == c_period){
                ret_HC = HC;
                ret_SC = SC;
                result[c_exam][0] = tmp_period;
                result[c_exam][1] = tmp_room;
                break;
            }
            else if(i < n_periods-1)i++;
            else i = 0;
        }
        else if(j < n_rooms-1)j++;
        else j=0;
    }
    printf("HC = %d, SC = %d", ret_HC, ret_SC);
}
int Period_change(int result[][2], int n_exams, int n_periods, int n_rooms, int exam[][500], int period[][2], int room[][2], int PHC[][3], int RHC[], int HC, int SC, int IW[], int day[], int largest_exam[]){
    int ret_HC, ret_SC;
    int c_exam = rand()%n_exams;
    while(result[c_exam][0]==-1)c_exam = rand()%n_exams;
    int c_period = rand()%n_periods;
    // 総当たり用
    int i;
    if(c_period == n_periods-1)i=0;
    else i = c_period+1;

    // 選択した試験のもとの状態を記憶
    int tmp_period = result[c_exam][0];

    //適用
    result[c_exam][0] = c_period;
    ret_HC = count_HC(result, n_exams, n_periods, n_rooms, exam, period, room, PHC, RHC);
    ret_SC = count_SC(result, exam, n_exams, n_periods, IW, day, largest_exam, room, n_rooms, period, 0);
    while(!((ret_HC < HC)||(ret_SC < SC && ret_HC <=HC))){
        //更新
        result[c_exam][0] = c_period;
        //計算
        ret_HC = count_HC(result, n_exams, n_periods, n_rooms, exam, period, room, PHC, RHC);
        ret_SC = count_SC(result, exam, n_exams, n_periods, IW, day, largest_exam, room, n_rooms, period, 0);
        if(i == c_period){//全部試してダメだったら
            result[c_exam][0] = tmp_period;
            ret_HC = HC;
            break;
        }
        //ダメだった場合の総当たり
        else if(i < n_periods-1)i++;
        else i = 0;
    }
    return ret_HC;
}
int Room_change(int result[][2], int n_exams, int n_periods, int n_rooms, int exam[][500], int period[][2], int room[][2], int PHC[][3], int RHC[], int HC, int SC, int IW[], int day[], int largest_exam[]){
    int ret_HC, ret_SC;
    int c_exam = rand()%n_exams;
    while(result[c_exam][0]==-1)c_exam = rand()%n_exams;
    int c_room = rand()%n_rooms;
    // 総当たり用
    int i;
    if(c_room == c_room-1)i=0;
    else i = c_room+1;

    // 選択した試験のもとの状態を記憶
    int tmp_room = result[c_exam][0];

    //適用
    result[c_exam][0] = c_room;
    ret_HC = count_HC(result, n_exams, n_periods, n_rooms, exam, period, room, PHC, RHC);
    ret_SC = count_SC(result, exam, n_exams, n_periods, IW, day, largest_exam, room, n_rooms, period, 0);
    while(!((ret_HC < HC)||(ret_SC < SC && ret_HC <=HC))){
        //更新
        result[c_exam][0] = c_room;
        //計算
        ret_HC = count_HC(result, n_exams, n_periods, n_rooms, exam, period, room, PHC, RHC);
        ret_SC = count_SC(result, exam, n_exams, n_periods, IW, day, largest_exam, room, n_rooms, period, 0);
        if(i == c_room){//全部試してダメだったら
            result[c_exam][0] = tmp_room;
            ret_HC = HC;
            break;
        }
        //ダメだった場合の総当たり
        else if(i < n_rooms-1)i++;
        else i = 0;
    }
    return ret_HC;
}
int Period_swap(int result[][2], int n_exams, int n_periods, int n_rooms, int exam[][500], int period[][2], int room[][2], int PHC[][3], int RHC[], int HC);
int Room_swap(int result[][2], int n_exams, int n_periods, int n_rooms, int exam[][500], int period[][2], int room[][2], int PHC[][3], int RHC[], int HC);
int Period_room_change(int result[][2], int n_exams, int n_periods, int n_rooms, int exam[][500], int period[][2], int room[][2], int PHC[][3], int RHC[], int HC, int SC, int IW[], int day[], int largest_exam[]){
    int ret_HC, ret_SC;
    // 試験、時間帯、教室を乱数で選択
    int c_exam = rand()%n_exams;
    c_exam = rand()%n_exams;
    int c_period = rand()%n_periods;
    // 総当たり用
        // 総当たり用
    int i;
    if(c_period == n_periods-1)i=0;
    else i = c_period+1;
    // 選択した試験のもとの状態を記憶
    int tmp_period = result[c_exam][0];
    int tmp_room = result[c_exam][1];

    // 適用
    result[c_exam][0] = c_period;
    result[c_exam][1] = rand()%n_rooms;
    ret_HC = count_HC(result, n_exams, n_periods, n_rooms, exam, period, room, PHC, RHC);
    ret_SC = count_SC(result, exam, n_exams, n_periods, IW, day, largest_exam, room, n_rooms, period, 0);

    while(!((ret_HC < HC)||(ret_SC < SC && ret_HC <=HC))){
        result[c_exam][0] = i;
        result[c_exam][1] = rand()%n_rooms;
        ret_HC = count_HC(result, n_exams, n_periods, n_rooms, exam, period, room, PHC, RHC);
        ret_SC = count_SC(result, exam, n_exams, n_periods, IW, day, largest_exam, room, n_rooms, period, 0);
        if(i == c_period){
            ret_HC = HC;
            result[c_exam][0] = tmp_period;
            result[c_exam][1] = tmp_room;
            break;
        }
        else if(i < n_periods-1)i++;
        else i = 0;
    }
    return ret_HC;
}
int Exam_swap_origin(int result[][2], int n_exams, int n_periods, int n_rooms, int exam[][500], int period[][2], int room[][2], int PHC[][3], int RHC[], int HC){
    int ret_HC;
    static int c_exam=0;
    // // 試験、時間帯、教室を乱数で選択
    // int c_exam = rand()%n_exams;
    while(result[c_exam][0]!=-1 && c_exam < n_exams){
        c_exam++;
    }

    if(c_exam==n_exams){
        c_exam=0;
        return HC;
    }

    int c_period = rand()%n_periods;
    int c_room = rand()%n_rooms;
    // 総当たり用
    int i;
    int j;
    if(c_period == n_periods-1)i=0;
    else i = c_period+1;
    if(c_room == n_rooms-1)j=0;
    else j = c_room+1;

    // 選択した試験のもとの状態を記憶
    int tmp_period = result[c_exam][0];
    int tmp_room = result[c_exam][1];

    // 適用
    result[c_exam][0] = c_period;
    result[c_exam][1] = c_room;
    ret_HC = count_HC(result, n_exams, n_periods, n_rooms, exam, period, room, PHC, RHC);

    while(HC < ret_HC){
        // 更新
        result[c_exam][0] = i;
        result[c_exam][1] = j;
        ret_HC = count_HC(result, n_exams, n_periods, n_rooms, exam, period, room, PHC, RHC);
        if(j == c_room){//全部屋だめなら
            if(i == c_period){
                result[c_exam][0] = tmp_period;
                result[c_exam][1] = tmp_room;
                ret_HC = HC;
                break;
            }
            else if(i < n_periods-1)i++;
            else i = 0;
        }
        else if(j < n_rooms-1)j++;
        else j=0;
    }
    c_exam++;
    return ret_HC;
}
void neighbor(int result[][2], int n_exams, int n_periods, int n_rooms, int exam[][500], int period[][2], int room[][2], int PHC[][3], int RHC[], int HC, int SC, int IW[], int day[], int largest_exam[]){
    int ch = rand()%5;
    printf("ch=%d\n",ch);
    switch(ch){
        case 0:
            Exam_swap(result, n_exams, n_periods, n_rooms, exam, period, room, PHC,RHC,HC,SC,IW,day,largest_exam);
            break;
        case 1:
            Period_change(result, n_exams, n_periods, n_rooms, exam, period, room, PHC,RHC,HC,SC,IW,day,largest_exam);
            break;
        case 2:
            Room_change(result, n_exams, n_periods, n_rooms, exam, period, room, PHC,RHC,HC,SC,IW,day,largest_exam);
            break;
        case 3:
            Period_room_change(result, n_exams, n_periods, n_rooms, exam, period, room, PHC,RHC,HC,SC,IW,day,largest_exam);
            break;
    }
}

void swap_num(int *a, int *b){
    int tmp = *a;
    *a = *b;
    *b = tmp;
}
void swap_result(int result[][2], int x, int y){
    swap_num(&result[x][0],&result[y][0]);
    swap_num(&result[x][1],&result[y][1]);
}

int linear_search(int* array, size_t size, int value)
{
    for (int i = 0; i < size; i++)
    {
        /* code */
        if(array[i]==value)return 1;
    }
    return 0;
}




