#include <iostream>
#include <fstream>
#include <vector>

struct Header {
    char  idLength;
    char  colorMapType;
    char  dataTypeCode;
    short colorMapOrigin;
    short colorMapLength;
    char  colorMapDepth;
    short xOrigin;
    short yOrigin;
    short width;
    short height;
    char  bitsPerPixel;
    char  imageDescriptor;
};

struct Pixel {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
    Pixel(char b, char g, char r){ // should i put unsigned char?
        blue = b;
        green = g;
        red = r;
    }
};

struct Image {
    Header header;
    std::vector<Pixel> imageData;
    Image();
    Image(Header header, std::vector<Pixel> imageData){
        this->header = header;
        this->imageData = imageData;
    }
};


//// I can just pass the same header! no need to create new one

void printHeaderValues(Header* headerObject){
    std::cout << "\nID Length: " << (int)headerObject->idLength;
    std::cout << "\nColor Map Type: " << (int)headerObject->colorMapType;
    std::cout << "\nData Type Code: " << (int)headerObject->dataTypeCode;
    std::cout << "\nColor Map Origin: " << headerObject->colorMapOrigin;
    std::cout << "\nColor Map Length: " << headerObject->colorMapLength;
    std::cout << "\nColor Map Depth: " << (int)headerObject->colorMapDepth;
    std::cout << "\nX Origin: " << headerObject->xOrigin;
    std::cout << "\nY Origin: " << headerObject->yOrigin;

    std::cout << "\nImage Width: " << headerObject->width;
    std::cout << "\nImage Height: " << headerObject->height;

    std::cout << "\nBits per Pixel: " << (int)headerObject->bitsPerPixel;
    std::cout << "\nImage Descriptor: " << (int)headerObject->imageDescriptor;
}

Image readFile(std::string filename){

    std::vector<Pixel> vectorOfPixels;
    Header headerObject; // create headerObject

    std::ifstream myFile(filename, std::ios::binary);
    if (myFile.is_open()){
        std::cout << "\nFile was opened";
        myFile.read(&headerObject.idLength, sizeof(headerObject.idLength));
        myFile.read(&headerObject.colorMapType, sizeof(headerObject.colorMapType));
        myFile.read(&headerObject.dataTypeCode, sizeof(headerObject.dataTypeCode));
        myFile.read((char*)&headerObject.colorMapOrigin, sizeof(headerObject.colorMapOrigin));
        myFile.read((char*)&headerObject.colorMapLength, sizeof(headerObject.colorMapLength));
        myFile.read(&headerObject.colorMapDepth, sizeof(headerObject.colorMapDepth));
        myFile.read((char*)&headerObject.xOrigin, sizeof(headerObject.xOrigin));
        myFile.read((char*)&headerObject.yOrigin, sizeof(headerObject.yOrigin));

        myFile.read((char*)&headerObject.width, sizeof(headerObject.width));
        myFile.read((char*)&headerObject.height, sizeof(headerObject.height));

        myFile.read(&headerObject.bitsPerPixel, sizeof(headerObject.bitsPerPixel));
        myFile.read(&headerObject.imageDescriptor, sizeof(headerObject.imageDescriptor));

        int numberOfPixels = headerObject.width * headerObject.height;
        int numberOfBytes = numberOfPixels*3;

        unsigned char test[numberOfBytes];
        myFile.read((char*)&test, numberOfBytes);

        for (int i = 0; i < numberOfBytes; i+=3) {
            Pixel* current = new Pixel(test[i], test[i+1], test[i+2]);
            vectorOfPixels.push_back(*current);
        }

    } else {
        std::cout << "Could not open file";
    }

    Image result(headerObject, vectorOfPixels); // I know I am copying instead of using pointer
    return result;

}

void createFile(Image* imageToCreate, std::string filename){

    Header headerObject = imageToCreate->header;

    std::ofstream createFile(filename, std::ios::binary);

    if (createFile.is_open()){
        std::cout << "\nFile is opened/created";
        createFile.write(&headerObject.idLength, sizeof(headerObject.idLength));
        createFile.write(&headerObject.colorMapType, sizeof(headerObject.colorMapType));

        createFile.write(&headerObject.dataTypeCode, sizeof(headerObject.dataTypeCode));
        createFile.write((char*)&headerObject.colorMapOrigin, sizeof(headerObject.colorMapOrigin));
        createFile.write((char*)&headerObject.colorMapLength, sizeof(headerObject.colorMapLength));
        createFile.write(&headerObject.colorMapDepth, sizeof(headerObject.colorMapDepth));
        createFile.write((char*)&headerObject.xOrigin, sizeof(headerObject.xOrigin));
        createFile.write((char*)&headerObject.yOrigin, sizeof(headerObject.yOrigin));
        //                      writing             how many bytes we need to write it
        createFile.write((char*)&headerObject.width, sizeof(headerObject.width));
        createFile.write((char*)&headerObject.height, sizeof(headerObject.height));

        createFile.write(&headerObject.bitsPerPixel, sizeof(headerObject.bitsPerPixel));
        createFile.write(&headerObject.imageDescriptor, sizeof(headerObject.imageDescriptor));

        for(Pixel pxl : imageToCreate->imageData){
            createFile.write((char*)&pxl, 3);
        }
        std::cout << "\nImage file created successfully\n";

    } else {
        std::cout << "\nFile could not be opened";
    }

}

Image multiply(Image* topLayer, Image* bottomLayer){
    Header headerObject = topLayer->header;
    std::vector<Pixel> imageData;
    std::cout << "\nMultiplying...\n";
    for(int i = 0; i < topLayer->imageData.size(); i++){
        // turn color char value to int from 0 to 255. Divide by 255 to get value between 0-1. multiply by int red value of bottom layer. Cast result back to char.
        char blue = (char)(((((float)topLayer->imageData[i].blue)/255) * (float)bottomLayer->imageData[i].blue) + (double)0.5);
        char green = (char)(((((float)topLayer->imageData[i].green)/255) * (float)bottomLayer->imageData[i].green) + (double)0.5);
        char red = (char)(((((float)topLayer->imageData[i].red)/255) * (float)bottomLayer->imageData[i].red) + (double)0.5);

        Pixel pxl(blue, green, red);
        imageData.push_back(pxl);
    }
    std::cout << "Image was multiplied!";
    return Image(headerObject, imageData);
}

Image subtract(Image* topLayer, Image* bottomLayer){
    Header headerObject = topLayer->header;
    std::vector<Pixel> imageData;
    std::cout << "\nSubtracting...\n";
    for(int i = 0;i < topLayer->imageData.size(); i++){

        int b = ( ((int)bottomLayer->imageData[i].blue) - ((int)topLayer->imageData[i].blue));
        int g = (((int)bottomLayer->imageData[i].green) - ((int)topLayer->imageData[i].green));
        int r = (((int)bottomLayer->imageData[i].red) - ((int)topLayer->imageData[i].red));

        char blue = (char)(b < 0 ? 0 : b);
        char green = (char)(g < 0 ? 0 : g);
        char red = (char)(r < 0 ? 0 : r);

        Pixel pxl(blue, green, red);
        imageData.push_back(pxl);
    }
    std::cout << "Subtracted!\n";
    return Image(headerObject, imageData);
}

Image screen(Image* topLayer, Image* bottomLayer){
    Header headerObject = topLayer->header;
    std::vector<Pixel> imageData;
    for(int i = 0; i < topLayer->imageData.size(); i++){
        Pixel top = topLayer->imageData[i];
        Pixel bottom = bottomLayer->imageData[i];

        char blue = (char)(((1 - ((1-((float)top.blue/255))*(1-((float )bottom.blue/255))))*255)+(double)0.5);
        char green = (char)(((1 - ((1-((float)top.green/255))*(1-((float )bottom.green/255))))*255)+(double)0.5);
        char red = (char)(((1 - ((1-((float)top.red/255))*(1-((float )bottom.red/255))))*255)+(double)0.5);

        Pixel pxl(blue, green, red);
        imageData.push_back(pxl);

    }
    return Image(headerObject, imageData);
}

Image overlay(Image* topLayer, Image* bottomLayer){
    Header headerObject = topLayer->header;
    std::vector<Pixel> imageData;
    for(int i = 0;i < topLayer->imageData.size(); i++){

        Pixel top = topLayer->imageData[i];
        Pixel bottom = bottomLayer->imageData[i];

        float b = ((float)bottom.blue/255);
        float g = ((float)bottom.green/255);
        float r = ((float)bottom.red/255);
        //  Overlay method -> B is the bottomLayer pixel. If B is less than or equal to 0.5 (from 0 - 1) then C = 2*A*B, else C = 1 - 2*(1-A)*(1-B)
        char blue = (b <= 0.5f ? (char)((2*((float)top.blue/255)*(b))*255) : (char)((1 - (2*(1 - ((float)top.blue/255)) * (1 - b)))*255));
        char green = (g <= 0.5f ? (char)((2*((float)top.green/255)*(g))*255) : (char)((1 - (2*(1 - ((float)top.green/255)) * (1 - g)))*255));
        char red = (r <= 0.5f ? (char)((2*((float)top.red/255)*(r))*255) : (char)((1 - (2*(1 - ((float)top.red/255)) * (1 - r)))*255));

        Pixel pxl(blue, green, red);
        imageData.push_back(pxl);

    }
    return Image(headerObject, imageData);
}

Image addColor(Image* image){ // add 200 green
    std::vector<Pixel> imageData;
    for(int i = 0; i < image->imageData.size(); i++){

        int g = (int)image->imageData[i].green + 200;
        char green = (char)(g > 255 ? 255 : g);

        Pixel pxl(image->imageData[i].blue, green, image->imageData[i].red);
        imageData.push_back(pxl);
    }
    return Image(image->header, imageData);
}

Image scale(Image* image){
    std::vector<Pixel> imageData;
    for(int i = 0; i < image->imageData.size(); i++){
        int r = (int)(image->imageData[i].red)*4; // scale red by 4
        char red = (char)(r > 255 ? 255 : r);
        // blue set to 0
        Pixel pxl(0, image->imageData[i].green, red);
        imageData.push_back(pxl);
    }
    return Image(image->header, imageData);
}

Image oneChannel(Image* image, std::string color){
    std::vector<Pixel> imageData;
    for (int i = 0; i < image->imageData.size(); i++) {

        Pixel currentPixel = image->imageData[i];

        char channel = (char)(color == "red" ? currentPixel.red : (color == "green" ? currentPixel.green : currentPixel.blue));

        Pixel pxl(channel, channel, channel);
        imageData.push_back(pxl);
    }
    return Image(image->header, imageData);
}

Image combineLayers(Image* blueLayer, Image* greenLayer, Image* redLayer){
    std::vector<Pixel> imageData;
    for(int i = 0; i < blueLayer->imageData.size(); i++){
        char blue = (char)blueLayer->imageData[i].blue;
        char green = (char)greenLayer->imageData[i].green;
        char red = (char)redLayer->imageData[i].red;
        Pixel pxl(blue, green ,red);
        imageData.push_back(pxl);
    }
    return Image(blueLayer->header, imageData);
}

Image rotate(Image* image){
    std::vector<Pixel> imageData;
    for(int i = (int)image->imageData.size(); i >= 0; i--){
        char blue = (char)image->imageData[i].blue;
        char green = (char)image->imageData[i].green;
        char red = (char)image->imageData[i].red;
        Pixel pxl(blue, green, red);
        imageData.push_back(pxl);
    }
    return Image(image->header, imageData);
}

Image combineAll(Image* imageOne, Image* imageTwo, Image* imageThree, Image* imageFour){
    // Setting new dimensions for new image - Making header of new image
    int newImageWidth = imageOne->header.width + imageTwo->header.width;
    int newImageHeight = imageOne->header.height + imageThree->header.height;
    Header headerObject = imageOne->header;
    headerObject.width = (short)newImageWidth;
    headerObject.height = (short)newImageHeight;

    std::vector<Pixel> imageData; // vector of pixel for new image

    int indexOne = 0;
    int indexTwo = 0;
    int indexThree = 0;
    int indexFour = 0;
    int counterBottom = 1;
    int counterTop = 1;

    char blue, green, red;

    for(int i = 0; i < ((int)imageOne->imageData.size() * 4); i++){
        if(i <= imageOne->imageData.size() * 2){ // if we are on bottom half of new image
            if(i <= imageOne->header.width * counterBottom){ // if we are one left side image of bottom half
                blue = (char)imageOne->imageData[indexOne].blue;
                green = (char)imageOne->imageData[indexOne].green;
                red = (char)imageOne->imageData[indexOne].red;
                indexOne++;
            } else { // we are on right side image of bottom half
                blue = (char)imageTwo->imageData[indexTwo].blue;
                green = (char)imageTwo->imageData[indexTwo].green;
                red = (char)imageTwo->imageData[indexTwo].red;
                indexTwo++;
            }
            if(i == newImageWidth * ((counterBottom+1)/2) ) counterBottom += 2; // if we reached the border of right side, we start a new line from left
        } else { // we are on top half of image
            //           size of bottom half        +   offset from there
            if(i <= (imageOne->imageData.size() * 2) + (imageThree->header.width * counterTop)){ // if we are one left side image of top half
                blue = (char)imageThree->imageData[indexThree].blue;
                green = (char)imageThree->imageData[indexThree].green;
                red = (char)imageThree->imageData[indexThree].red;
                indexThree++;
            } else { // we are on right side image of bottom half
                blue = (char)imageFour->imageData[indexFour].blue;
                green = (char)imageFour->imageData[indexFour].green;
                red = (char)imageFour->imageData[indexFour].red;
                indexFour++;
            }
            if(i == ((imageOne->imageData.size() * 2) + (newImageWidth  * (counterTop+1)/2 ))) counterTop += 2; // if we reached the border of right side, we start a new line from left
        }

        Pixel pxl(blue, green, red);
        imageData.push_back(pxl);
    }
    return Image(headerObject, imageData);

}

int main() {

    // PART 1 - Multiply -> to combine layer1.tga (top layer) and pattern1.tga (bottom layer)
    // C = top * bottom  - layers

    Image layer1 = readFile("layer1.tga");
    Image pattern1 = readFile("pattern1.tga");

    Image result = multiply(&layer1, &pattern1);
    createFile(&result, "part1.tga");
    std::cout<<"\n///// PART 1 DONE /////\n";

    //PART 2 - Subtract top layer from bottom layer

    Image layer2 = readFile("layer2.tga");
    Image car = readFile("car.tga");

    result = subtract(&layer2, &car);
    createFile(&result, "part2.tga");
    std::cout<<"\n///// PART 2 DONE /////\n";

    // PART 3 -
    Image pattern2 = readFile("pattern2.tga");
    Image combined = multiply(&layer1, &pattern2);
    Image text = readFile("text.tga");
    result = screen(&text, &combined);
    createFile(&result, "part3.tga");
    std::cout << "\n///// PART 3 IS DONE //////\n";

    // PART 4
    Image circles = readFile("circles.tga");
    combined = multiply(&layer2, &circles);
    result = subtract(&pattern2, &combined);
    createFile(&result, "part4.tga");
    std::cout << "\n///// PART 4 DONE /////";

    // PART 5
    result = overlay(&layer1, &pattern1);
    createFile(&result, "part5.tga");
    std::cout << "\n///// PART 5 DONE //////\n";

    // PART 6
    result = addColor(&car);
    createFile(&result, "part6.tga");
    std::cout << "\n//// PART 6 DONE ////";

    // PART 7
    result = scale(&car);
    createFile(&result, "part7.tga");
    std::cout << "\n//// PART 7 DONE ////";

    // PART 8
    result = oneChannel(&car, "red");
    createFile(&result, "part8_r.tga");
    result = oneChannel(&car, "blue");
    createFile(&result, "part8_b.tga");
    result = oneChannel(&car, "green");
    createFile(&result, "part8_g.tga");
    std::cout << "\n//// PART 8 DONE ////";

    // PART 9
    Image blueLayer = readFile("layer_blue.tga");
    Image greenLayer = readFile("layer_green.tga");
    Image redLayer = readFile("layer_red.tga");

    result = combineLayers(&blueLayer, &greenLayer, &redLayer);
    createFile(&result, "part9.tga");
    std::cout << "\n//// PART 9 DONE ////";

    // PART 10
    Image text2 = readFile("text2.tga");
    result = rotate(&text2);
    createFile(&result, "part10.tga");
    std::cout << "\n//// PART 10 DONE ////";

    // EXTRA CREDIT - combine car.tga - circles.tga - pattern1.tga - text.tga
    result = combineAll(&text, &pattern1, &car, &circles);
    createFile(&result, "extracredit.tga");
    std::cout << "\n//// EXTRA CREDIT DONE ////";


    return 0;
}
















///// OUTPUT VALUES OF HEADER
//std::cout << "\nID Length: " << (int)headerObject.idLength;
//std::cout << "\nColor Map Type: " << (int)headerObject.colorMapType;
//std::cout << "\nData Type Code: " << (int)headerObject.dataTypeCode;
//std::cout << "\nColor Map Origin: " << headerObject.colorMapOrigin;
//std::cout << "\nColor Map Length: " << headerObject.colorMapLength;
//std::cout << "\nColor Map Depth: " << (int)headerObject.colorMapDepth;
//std::cout << "\nX Origin: " << headerObject.xOrigin;
//std::cout << "\nY Origin: " << headerObject.yOrigin;
//
//std::cout << "\nImage Width: " << headerObject.width;
//std::cout << "\nImage Height: " << headerObject.height;
//
//std::cout << "\nBits per Pixel: " << (int)headerObject.bitsPerPixel;
//std::cout << "\nImage Descriptor: " << (int)headerObject.imageDescriptor;
//
//std::cout << "\nSize of header is " << sizeof(Header) << " bytes";
