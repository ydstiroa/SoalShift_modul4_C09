#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

static const char *dirpath = "/home/najaslanardo/Documents/shift4";
const char charlist[] = "qE1~ YMUR2\"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|\\8s;g<{3.u*W-0";



void dekripsi(char * dekrip){
    if(strcmp("..", dekrip) ==0 || strcmp(".", dekrip)==0){
        return;
    }
    for(int i=0;i<strlen(dekrip);i++){
        for(int j=0;j<94;j++){
            if(dekrip[i]==charlist[j]){
                dekrip[i]= charlist[(j+77)%94];
                break;
            }
        }
    }
}

void enkripsi(char * enkrip){
    if(strcmp("..", enkrip) ==0 || strcmp(".", enkrip)==0){
        return;
    }
    for(int i=0;i<strlen(enkrip);i++){
        for(int j=0;j<94;j++){
            if(enkrip[i]==charlist[j]){
                enkrip[i]= charlist[(j+17)%94];
                break;
            }
        }
    }
}

static int xmp_getattr(const char *path, struct stat *stbuf)
{
    int res;
    char fpath[1000];
    char enc[1000];
    sprintf(enc, "%s", path);
    enkripsi(enc);

    printf("\tAttributes of %s requested\n", enc);
    sprintf(fpath, "%s%s", dirpath, enc);
    stbuf->st_uid = getuid();
    stbuf->st_gid = getgid();
    stbuf->st_atime = time( NULL );
    stbuf->st_mtime = time(NULL);
    if ( strcmp( path, "/" ) == 0 )
	{
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 4;
	}
	else
	{
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 4;
		stbuf->st_size = 1024;
	}
    res = lstat(fpath, stbuf);

    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi)
{
    char fpath[1000];
    char temp[1000];
    char enc[1000];
    sprintf(enc, "%s", path);
    enkripsi(enc);
    sprintf(fpath, "%s%s", dirpath, enc);
    printf("AKSES %s\n", fpath);

    DIR *dp;
    struct dirent *de;

    (void)offset;
    (void)fi;

    dp = opendir(fpath);
    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL)
    {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        strcpy(temp, de->d_name);
        dekripsi(temp);
        int res = (filler(buf, temp, &st, 0));
        if (res != 0)
            break;
    }

    closedir(dp);
    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi)
{
    char fpath[1000];
    
    int res = 0;
    int fd = 0;
    printf("Reading file %s\n", fpath);
    (void)fi;

    fd = open(fpath, O_RDONLY);
    if (fd == -1)
        return -errno;

    res = pread(fd, buf, size, offset);

    close(fd);
    return res;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
    int res;

    /* On Linux this could just be 'mknod(path, mode, rdev)' but this
       is more portable */
    char fpath[1000];
    char name[1000];
    sprintf(name,"%s",path);
    enkripsi(name);
    sprintf(fpath, "%s%s",dirpath,name);
    if (S_ISREG(mode)) {
        res = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
        if (res >= 0)
            res = close(res);
    } else if (S_ISFIFO(mode))
        res = mkfifo(fpath, mode);
    else
        res = mknod(fpath, mode, rdev);
    if (res == -1)
        return -errno;

    return 0;
}


static int xmp_mkdir(const char *path, mode_t mode)
{
    int res;
    char fpath[1000];
    char name[1000];
    sprintf(name,"%s",path);
    enkripsi(name);
    sprintf(fpath, "%s%s",dirpath,name);
    res = mkdir(fpath, mode);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_rmdir(const char *path)
{
    int res;
    char fpath[1000];
    char name[1000];
    sprintf(name,"%s",path);
    enkripsi(name);
    sprintf(fpath, "%s%s",dirpath,name);
    res = rmdir(fpath);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_rename(const char *from, const char *to)
{
    int res;
    char new_from[1000];
    char new_to[1000];
    char name1[1000];
    char name2[1000];
    sprintf(name1,"%s",from);
    sprintf(name2,"%s",to);
    enkripsi(name1);
    enkripsi(name2);
    sprintf(new_from,"%s%s",dirpath,name1);
    sprintf(new_to,"%s%s",dirpath,name2);
    res = rename(new_from, new_to);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_chmod(const char *path, mode_t mode)
{
    int res;
    char fpath[1000];
    char name[1000];
    sprintf(name,"%s",path);
    enkripsi(name);
    sprintf(fpath, "%s%s",dirpath,name);
    res = chmod(fpath, mode);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_chown(const char *path, uid_t uid, gid_t gid)
{
    int res;
    char fpath[1000];
    char name[1000];
    sprintf(name,"%s",path);
    enkripsi(name);
    sprintf(fpath, "%s%s",dirpath,name);
    res = lchown(fpath, uid, gid);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_create(const char *path, mode_t mode,
		      struct fuse_file_info *fi)
{
	int res;

    char dpath[1000];
    char fpath[1000];
    char temp[1000] = "/YOUTUBER";
    
    sprintf(dpath,"%s",path);
    if(strlen(dpath)!=9 && strcmp(dpath,temp) == 0){
        strcat(dpath,".iz1");
        sprintf(fpath, "%s%s",dirpath,dpath);
        res = creat(fpath,0640);
    }else{
        sprintf(fpath, "%s%s",dirpath,dpath);
        res = creat(fpath,mode);
    }
    
	if (res == -1)
		return -errno;

	
	return 0;
}

static int xmp_truncate(const char *path, off_t size)
{
    int res;
    char fpath[1000];
    char name[1000];
    sprintf(name,"%s",path);
    enkripsi(name);
    sprintf(fpath, "%s%s",dirpath,name);
    res = truncate(fpath, size);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_utimens(const char *path, const struct timespec ts[2])
{
    int res;
    struct timeval tv[2];

    char fpath[1000];
    char name[1000];
    sprintf(name,"%s",path);
    enkripsi(name);
    sprintf(fpath, "%s%s",dirpath,name);

    tv[0].tv_sec = ts[0].tv_sec;
    tv[0].tv_usec = ts[0].tv_nsec / 1000;
    tv[1].tv_sec = ts[1].tv_sec;
    tv[1].tv_usec = ts[1].tv_nsec / 1000;

    res = utimes(fpath, tv);
    if (res == -1)
        return -errno;

    return 0;
}
static int xmp_open(const char *path, struct fuse_file_info *fi)
{
    int res;
    char fpath[1000];
    char name[1000];
    sprintf(name,"%s",path);
    enkripsi(name);
    sprintf(fpath, "%s%s",dirpath,name);
    res = open(fpath, fi->flags);
    if (res == -1)
        return -errno;

    close(res);
    return 0;
}

static struct fuse_operations xmp_oper = {
    .getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.mknod		= xmp_mknod,
	.mkdir		= xmp_mkdir,
	.rmdir		= xmp_rmdir,
	.rename		= xmp_rename,
	.chmod		= xmp_chmod,
	.chown		= xmp_chown,
	.truncate	= xmp_truncate,
	.utimens	= xmp_utimens,
	.open		= xmp_open,
	.read		= xmp_read,
	.create	 	= xmp_create,
};



int main(int argc, char *argv[])
{
    umask(0);
    return fuse_main(argc, argv, &xmp_oper, NULL);
}
