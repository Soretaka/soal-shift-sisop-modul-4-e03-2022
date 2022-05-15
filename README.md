# Soal-shift-sisop-modul-4-E03-2022
1. Daniel Hermawan (5025201087)
2. Florentino Benedictus (5025201222)
3. Anak Agung Yatestha Parwata (5025201234)

## soal no 1
Anya adalah adalah seorang programmer wibu yang suka mengoleksi anime. Dia sangat senang membuat program yang dikolaborasikan dengan anime. Suatu hari, Anya sedang ingin membuat system dengan ketentuan berikut:

a. Semua direktori dengan awalan “Animeku_” akan terencode dengan ketentuan semua file yang terdapat huruf besar akan ter encode dengan atbash cipher dan huruf kecil akan terencode dengan rot13
    
    Contoh : 
    “Animeku_/anya_FORGER.txt” → “Animeku_/naln_ULITVI.txt”

b. Semua direktori di-rename dengan awalan “Animeku_”, maka direktori tersebut akan menjadi direktori ter-encode dengan ketentuan sama dengan 1a.

c. Apabila direktori yang terenkripsi di-rename menjadi tidak ter-encode, maka isi direktori tersebut akan terdecode.

d. Setiap data yang terencode akan masuk dalam file “Wibu.log” 
Contoh isi: 
RENAME terenkripsi /home/[USER]/Downloads/hai --> /home/[USER]/Downloads/Animeku_hebat 
RENAME terdecode /home/[USER]/Downloads/Animeku_ --> /home/[USER]/Downloads/Coba

e. Metode encode pada suatu direktori juga berlaku terhadap direktori yang ada di dalamnya.(rekursif)
Note : filesystem berfungsi normal layaknya linux pada umumnya, Mount source (root) filesystem adalah directory /home/[USER]/Documents, dalam penamaan file ‘/’ diabaikan, dan ekstensi tidak perlu di-encode

Referensi : https://www.base64encode.org/ https://rot13.com/

untuk menyelesaikan soal 1a,1b,1c,dan 1e diatas, berikut merupakan fungsi encode dan decodenya:
```c
char atbash[26]={"ZYXWVUTSRQPONMLKJIHGFEDCBA"};
char alphabetup[26]={"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
char alphabetlow[26]={"abcdefghijklmnopqrstuvwxyz"};
char rot13[26]={"nopqrstuvwxyzabcdefghijklm"};

char bash (char alphabet){
    for(int i = 0 ; i < 26 ; i++) if (alphabet == alphabetup[i]) return atbash[i];
    return '.';
}
char rot (char alphabet){
    for(int i = 0 ; i < 26 ; i++) if (alphabet == alphabetlow[i]) return rot13[i];    
    return '.';
}
void encode1(char* strEnc1,const char *path) { //encrypt ANimeku
	char oldPath[SEGMENT];
	char newPath[SEGMENT];
	strcpy(oldPath,"");
	strcpy(newPath,"");
	strcpy(oldPath,path);
	strcat(oldPath,"/");
	strcat(oldPath,strEnc1);
	if(strcmp(strEnc1, ".") == 0 || strcmp(strEnc1, "..") == 0)
        return;
    int strLength = strlen(strEnc1);
    int LastDot=-1;
    for (int i = 0 ; i < strLength; i++){
        if(strEnc1[i]=='.')LastDot=i;
    }
    for(int i = 0; i < strLength; i++) {
		if(strEnc1[i] == '/') 
            continue;
		if(strEnc1[i]=='.' && i==LastDot)
            break;
        
		if(strEnc1[i]>='A'&&strEnc1[i]<='Z')
            strEnc1[i] = 'Z'+'A'-strEnc1[i];
        if(strEnc1[i]>='a'&&strEnc1[i]<='z')
            strEnc1[i] = 'a'+ (strEnc1[i] - 'a' + 13) %26 ;
    }
	strcpy(newPath,path);
	strcat(newPath,"/");
	strcat(newPath,strEnc1);
	logging1("terenkripsi",oldPath, newPath);
}

void decode1(char * strDec1,const char *path){ //decrypt Animeku_
	char oldPath[SEGMENT];
	char newPath[SEGMENT];
	strcpy(oldPath,"");
	strcpy(newPath,"");
	strcpy(oldPath,path);
	strcat(oldPath,"/");
	strcat(oldPath,strDec1);
	if(strcmp(strDec1, ".") == 0 || strcmp(strDec1, "..") == 0 || strstr(strDec1, "/") == NULL) 
        return;
    
    int strLength = strlen(strDec1), s=0;
	for(int i = strLength; i >= 0; i--){
		if(strDec1[i]=='/')break;

		if(strDec1[i]=='.'){//nyari titik terakhir
			strLength = i;
			break;
		}
	}
	for(int i = 0; i < strLength; i++){
		if(strDec1[i]== '/'){
			s = i+1;
			break;
		}
	}
    for(int i = s; i < strLength; i++) {
		if(strDec1[i] =='/'){
            continue;
        }
        if(strDec1[i]>='A'&&strDec1[i]<='Z'){
            strDec1[i] = 'Z'+'A'-strDec1[i];
        }
        if(strDec1[i]>='a'&&strDec1[i]<='z'){
            strDec1[i] = 'a' + (strDec1[i] - 'a' + 13) % 26 ;
        }
	}
	strcpy(newPath,path);
	strcat(newPath,"/");
	strcat(newPath,strDec1);
	logging1("terdecode",oldPath, newPath);
}

```

Kemudian, untuk penggantian namanya, kita mengubah fungsi 'xmp_readdir' menjadi seperti berikut

```c
static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi){ 
	char * strToEnc1 = strstr(path, prefix);
    
	if(strToEnc1 != NULL) {
        decode1(strToEnc1,path);
    }

	char newPath[1000];
	if(strcmp(path,"/") == 0){
		path = directoryPath;
		sprintf(newPath, "%s", path);
	} else 
        sprintf(newPath, "%s%s", directoryPath, path);

	int result = 0;
	struct dirent *dir;
	DIR *dp;
	(void) fi;
	(void) offset;
	dp = opendir(newPath);
	if (dp == NULL) return -errno;
	
	//buat loop yang ada di dalam directory
	while ((dir = readdir(dp)) != NULL) { 
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = dir->d_ino;
		st.st_mode = dir->d_type << 12;
		if(strToEnc1 != NULL){
			//encode yang ada di dalam directory sekarang
			encode1(dir->d_name,path); 
        }
		
		result = (filler(buf, dir->d_name, &st, 0));
		if(result!=0) break;
	}

	closedir(dp);
	return 0;
}

```

yang bertugas untuk mengecek apakah file tersebut mengandung "Animeku_" adalah variabel strToEnc1 (pengecekan menggunakan strstr, dimana jika tidak terdapat kata Animeku_ maka strstr akan mengembalikan nilai NULL). Lalu

```c
	while ((dir = readdir(dp)) != NULL) { 
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = dir->d_ino;
		st.st_mode = dir->d_type << 12;
		if(strToEnc1 != NULL){
			//encode yang ada di dalam directory sekarang
			encode1(dir->d_name,path); 
        }
		
		result = (filler(buf, dir->d_name, &st, 0));
		if(result!=0) break;
	}
```
digunakan untuk mengecek sub directory, jika perlu di encode maka akan di encode.

Selain penambahan kodingan pada fungsi 'xmp_readdir', kita juga melakukan penyesuaian mendecode pada fungsi 'xmp_getattr' karena pada saat fuse ingin menjalankan fungsi tersebut kita membutuhkan nama file yang asli agar bisa dicari dan ditampilkan. Berikut fungsinya setelah diubah:

```c
static  int  xmp_getattr(const char *path, struct stat *stbuf){
	char * strToEnc1 = strstr(path, prefix);
	
	if(strToEnc1 != NULL){
		decode1(strToEnc1,path);
    }

	char newPath[1000];
	int result;
	sprintf(newPath,"%s%s", directoryPath, path);
	result = lstat(newPath, stbuf);
	if (result == -1)
		return -errno;
	return 0;
}

```


Lalu untuk soal 1d. pencatatan log menggunakan fungsi sebagai berikut:
```c
void logging1(const char* kind, const char* old, char* new) {
	
	FILE * logFile = fopen(WibuLogPath, "a");
	if (logFile == NULL) printf("Error : errno = '%s'.\n",strerror(errno));
	else fprintf(logFile, "RENAME %s %s --> %s\n",kind, old, new);
    fclose(logFile);
}
```

fungsi tersebut diletakan pada fungsi encode dan decode untuk mencatat setiap kali encode atau decode terjadi.

#### screenshot
soal 1a,1b,1d, dan 1e

![Fuse Berjalan](/uploads/c1884ed6e287b823c07dd7fd54a95ca7/Screenshot_from_2022-05-15_17-04-19.png)

Fuse berjalan

![Encode sesuai soal](/uploads/8ed73e50b5a95ff28c482d33e62cef5d/Screenshot_from_2022-05-15_17-05-12.png)

Encode sesuai soal

![Ketika file direname, maka terdecode otomatis](/uploads/bba1a750dadbd6d89c31fcfaeae99ab6/Screenshot_from_2022-05-15_17-08-30.png)

Ketika file direname, maka terdecode otomatis

soal 1d

![hasil log](/uploads/70adc3a045b1e955741591cde0db294b/Screenshot_from_2022-05-15_17-18-33.png)

hasil log


# Soal 2
Saat Anya sedang sibuk mengerjakan programnya, tiba-tiba Innu datang ke rumah Anya untuk mengembalikan barang yang dipinjamnya. Innu adalah programmer jenius sekaligus teman Anya. Ia melihat program yang dibuat oleh Anya dan membantu Anya untuk menambahkan fitur pada programnya dengan ketentuan sebagai berikut :

## 2a
Jika suatu direktori dibuat dengan awalan “IAN_[nama]”, maka seluruh isi dari direktori tersebut akan terencode dengan algoritma Vigenere Cipher dengan key “INNUGANTENG” (Case-sensitive, Vigenere).

## Penyelesaian 
Untuk mengklasifikasikan apakah suatu direktori akan diencode atau didecode, kami membuat fungsi isAnimeku yang mengembalikan nilai true atau false. Ketika isAnimeku bernilai true, yaitu ketika terdapat substring "IAN_", maka direktori path yang dimasukkan akan diencode, sebaliknya ketika bernilai false maka akan di-decode.
Fungsi isIAN:
```
bool isIAN(const char *path) 
{
    for(int i=0;i<strlen(path)-4+1;i++)
        if(path[i] == 'I' && path[i+1] == 'A' && path[i+2] == 'N' && path[i+3] == '_') return 1;
    return 0;
}
```
Fungsi Encode dan Decode  Vigenere Cipher dengan key “INNUGANTENG” (case sensitive)
```
void encodeVig(char *s) 
{
    char key[] = "INNUGANTENG";
    for (int i=0;s[i];i++)
        if('A' <= s[i] && s[i] <= 'Z') s[i] = ((s[i]-'A'+(key[i%((sizeof(key)-1))]-'A'))%26)+'A';
        else if('a' <= s[i] && s[i] <= 'z') s[i] = ((s[i]-'a'+(key[i%((sizeof(key)-1))]-'A'))%26)+'a';
}

void decodeVig(char *s) 
{
    char key[] = "INNUGANTENG";
    for(int i=0;s[i];i++)
        if('A' <= s[i] && s[i] <= 'Z') s[i] = ((s[i]-'A'-(key[i%((sizeof(key)-1))]-'A')+26)%26)+'A';
        else if ('a' <= s[i] && s[i] <= 'z') s[i] = ((s[i]-'a'-(key[i%((sizeof(key)-1))]-'A')+26)%26)+'a';
}
```

## 2b
Jika suatu direktori di rename dengan “IAN_[nama]”, maka seluruh isi dari direktori tersebut akan terencode seperti pada no. 2a.

## Penyelesaian
Fungsi fuse untuk xmp_rename akan dipanggil ketika terdapat suatu direktori atau file yang direname. Pada fungsi ini, kita bisa mengecek apakah direktori diubah menjadi direktori yang akan di-encode atau di-decode.
Oleh karena itu, pada fungsi xmp_rename, kami mendeklarasikan string fpath (nama direktori sebelumnya) dan tpath (nama direktori sesudahnya).
fpath  atau tpath akan di-encode ketika memiliki substring "Animeku_". Jika tidak, direktori tersebut akan di-decode.
Pada fungsi fuse xmp_rename:
```
else if(isIAN(fpath) && !isIAN(tpath)){
        printf("[Mendekode %s.]\n", fpath);
        sistemLog(fpath, tpath, 2);
        int itung = decodeIAN(fpath, 1000);
        printf("[Total file yang terdekode: %d]\n", itung);
}else if(!isIAN(fpath) && isIAN(tpath)){
        printf("[Mengenkode %s.]\n", fpath);
        sistemLog(fpath, tpath, 1);
        int itung = encodeIAN(fpath, 1000);
        printf("[Total file yang terenkode: %d]\n", itung);
}
```

## 2c
Apabila nama direktori dihilangkan “IAN_”, maka isi dari direktori tersebut akan terdecode berdasarkan nama aslinya.

## Penyelesaian
Sama seperti pada nomor 2b

## 2d dan 2e
Untuk memudahkan dalam memonitor kegiatan yang dilakukan pada filesystem yang telah dibuat, ia membuat log system pada direktori “/home/[user]/hayolongapain_[kelompok].log”. Dimana log ini akan menyimpan daftar perintah system call yang telah dijalankan pada filesystem.

Karena Innu adalah seorang perfeksionis, ia membagi isi dari log systemnya menjadi 2 level, yaitu level INFO dan WARNING. Untuk log level WARNING, digunakan untuk mencatat syscall rmdir dan unlink. Sisanya, akan dicatat pada level INFO dengan format sebagai berikut : 

## Penyelesaian
Untuk menyelesaikan problem ini, kami membuat fungsi `logInfo` dan logWarning yang terintegrasi dengan fungsi `sistemLog` yang dapat mencatat aktivitas rename dari suatu direktori.

```
void sistemLog(char *dir1, char *dir2, int tipe) 
{
    char buff[1024], cmd[32];
    if(dir1[0]!='\0') strcpy(cmd, "RENAME"), sprintf(buff, "%s --> %s", dir1, dir2), logRename(cmd, tipe, buff), logIngfo(cmd,buff);
    else{
        if(tipe == 3){ //mkdir
            strcpy(cmd, "MKDIR"), sprintf(buff, "%s", dir2), logIngfo(cmd, buff);
        }else if(tipe == 4){ //rmdir
            strcpy(cmd, "RMDIR"), sprintf(buff, "%s", dir2), logWarning(cmd, buff);
        }else if(tipe == 5){ //unlink
            strcpy(cmd, "UNLINK"), sprintf(buff, "%s", dir2), logWarning(cmd, buff);
        }
    } 
    
}
```
Fungsi `logIngfo`
```
void logRename(char *cmd, int tipe, char *des) 
{
    time_t t = time(NULL);
    struct tm* lt = localtime(&t);
    char waktu[30];
    strftime(waktu, 30, "%d%m%Y-%H:%M:%S", lt);
    char logNya[1100];
    sprintf(logNya, "%s %s %s", cmd, tipe==1?"terenkripsi":"terdecode", des);
    FILE *out = fopen(fileLog, "a");
    fprintf(out, "%s\n", logNya);
    fclose(out);
    return;
}
```
Fungsi `logWarning`
```
void logWarning(char *cmd, char *des) 
{
    time_t t = time(NULL);
    struct tm* lt = localtime(&t); 
    char waktu[30];
    strftime(waktu, 30, "%d%m%Y-%H:%M:%S", lt); 
    char logNya[1100];
    sprintf(logNya, "WARNING::%s:%s::%s", waktu, cmd, des); 
    FILE *out = fopen(fileLogHayo, "a");
    fprintf(out, "%s\n", logNya);
    fclose(out);
}
```
