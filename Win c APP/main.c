#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
//gcc -o fileFormater fileFormater.c


uint8_t compute_crc(uint8_t *data, size_t length) {
    uint8_t crc = 0x00;  // Initial value
    for (size_t i = 0; i < length; i++) {   //i = 2 for 2 bytes of address
        crc += data[i];
    }
    return crc;
}

int main(int argc, char *argv[]) {
    uint8_t fileBuffer[5];
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *file = fopen(argv[1], "rw");
    if (!file) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    //fileBuffer[0] = (uint8_t)(start_add & 0x00FF);
    //fileBuffer[1] = (uint8_t)((start_add & 0xFF00)>>8);
    fseek(file, 0, SEEK_SET);
    fread(fileBuffer,sizeof(uint8_t),2,file);
    // Allocate buffer for file contents
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 2, SEEK_SET);   //2 btres for add

    uint8_t *data = (uint8_t *)malloc(file_size);
    if (!data) {
        perror("Memory allocation failed");
        fclose(file);
        return EXIT_FAILURE;
    }

    // Read file contents into buffer
    fread(data, 1, file_size-2, file);
    fclose(file);
    file = fopen(argv[2], "wb");
    if (!file) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }
    uint8_t crc = compute_crc(data, file_size-2);
    uint16_t start_add = 0x0700;
    uint16_t DataSize = file_size-2;
    
    
    fileBuffer[2] = (uint8_t)(DataSize & 0x00FF);
    fileBuffer[3] = (uint8_t)((DataSize & 0xFF00)>>8);
    fileBuffer[4] = crc;
    fseek(file, 0, SEEK_SET);
    fwrite(fileBuffer,sizeof(uint8_t),sizeof(fileBuffer),file);
    fwrite(data,sizeof(uint8_t),DataSize,file);


    fclose(file);
    printf("Chek sum(8 bit): 0x%02X\n", crc);
    printf("Start address: 0x%04X\n", start_add);
    printf("Size: 0x%04X\n", DataSize);

    // Free allocated memory
    free(data);

    return EXIT_SUCCESS;
}