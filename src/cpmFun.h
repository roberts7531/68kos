typedef struct Images{
    int number;
    char fileName[12];
}Image;
typedef struct Disks{
    char driveLetter;
    Image* image;
} Disk;

