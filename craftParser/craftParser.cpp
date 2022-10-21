// craftParser.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <vector>

typedef struct realmHeader_t {
    uint32_t numRecipes;
    uint32_t numCategories;
    uint32_t recipeListOffset;
    uint32_t catListOffset;
    uint32_t profListOffset;
};

typedef struct craftHeader_t {
    uint32_t version;
    uint32_t nameListSize;
    uint32_t numNames;
    uint32_t nameListOffset;
    realmHeader_t realmHeaders[3];
};

typedef struct craftProfession_t {
    uint16_t debug[3];
    uint16_t nameIndex;
    uint16_t indexes[200];
};

typedef struct craftProfessionData_t {
    uint16_t empty[201];
    craftProfession_t professions[19];
};

typedef struct craftCategory_t {
    uint32_t nameIndex;
    uint16_t recipeIds[50];
};

typedef struct craftMaterial_t {
    uint32_t nameIndex;
    uint16_t count;
    uint16_t baseMaterial;
};

//84 bytes
typedef struct craftItem_t {
    uint32_t nameIndex;
    uint32_t baseMaterial;
    uint32_t itemId;
    uint16_t icon;
    uint16_t skill;
    uint16_t materialLevel;
    uint16_t level;
    craftMaterial_t materials[8];
};

craftItem_t* ReadRecipes(std::ifstream& infile, uint32_t numRecipes, uint32_t offset) {
    std::vector<craftItem_t>* recipeList = new std::vector<craftItem_t>;

    uint32_t seekoffset = offset;
    for (int i = 0; i < numRecipes; i++) {
        craftItem_t current = {};

        infile.read((char*)&current, sizeof(craftItem_t));

        recipeList->push_back(current);
        seekoffset += sizeof(craftItem_t);
        
        infile.seekg(seekoffset);
    }
    std::cout << "recipe return offset: 0x" << std::hex << offset + offset << " recipe: 0x" << recipeList->at(1).itemId << std::endl;
    
    return recipeList->data();
}

craftCategory_t* ReadCategories(std::ifstream& infile, uint32_t numCategories, uint32_t recipeCount, uint32_t offset) {
    //Actually the C# is hardcoded 50 recipecount so replicate that here
    recipeCount = 50;
    
    std::vector<craftCategory_t>* catList = new std::vector<craftCategory_t>;
    std::vector<uint16_t> recipeIdList;// = new std::vector<uint16_t>;
    uint32_t seekoffset = offset;
    uint32_t recipeoffset = offset;
    for (int i = 0; i < numCategories; i++) {
        craftCategory_t current = {};
        //read the nameindex
        infile.read((char*)&current, sizeof(uint32_t));
        //set recipe offset to the current offset + 0x4 (1 uint32_t)
        recipeoffset = seekoffset + 0x4;
        //build the recipe id list
        for (int j = 0; j < recipeCount; j++) {
            //seek to the recipeoffset
            infile.seekg(recipeoffset);
            uint16_t temp;
            infile.read((char*)&temp, sizeof(uint16_t));
            //add to the list
            recipeIdList.push_back(temp);
            //offset by 0x4
            recipeoffset += 0x4;
        }
        
        for (int k = 0; k < recipeCount; k++) {
            current.recipeIds[k] = recipeIdList.at(k);
        }

        catList->push_back(current);
        seekoffset += sizeof(craftItem_t);

        infile.seekg(seekoffset);
    }
    std::cout << "cat return offset: 0x" << std::hex << offset + offset << " recipe: 0x" << catList->at(0).recipeIds[1] << std::endl;

    return catList->data();
}

int main()
{
    craftHeader_t header = craftHeader_t{};
    craftProfessionData_t realmProfessions[3] = {};
    craftItem_t* recipeList[3];
    craftCategory_t* categoryList[3];
    //Open the file
    std::ifstream myFile("edentdl.crf", std::ios::in | std::ios::binary);

    //read the header
    myFile.read((char*)&header, sizeof(craftHeader_t));

    //grab the craft profession data objects
    for (int i = 0; i < 3; i++) {
        //seek to the first profession struct
        myFile.seekg(header.realmHeaders[i].profListOffset);
        //read it into the object
        myFile.read((char*)&realmProfessions[i], sizeof(craftProfessionData_t));
    }
   
    //Get the recipe lists
    for (int i = 0; i < 3; i++) {
        std::cout << "recipe realm: " << i << " offset: 0x" << std::hex << header.realmHeaders[i].recipeListOffset << std::endl;
        myFile.seekg(header.realmHeaders[i].recipeListOffset);
        //put into array
        recipeList[i] = ReadRecipes(myFile, header.realmHeaders[i].numRecipes, header.realmHeaders[i].recipeListOffset);
    }

    //Get the category lists
    for (int i = 0; i < 3; i++) {
        std::cout << "cat realm: " << i << " offset: 0x" << std::hex << header.realmHeaders[i].catListOffset << std::endl;
        myFile.seekg(header.realmHeaders[i].catListOffset);
        //put into array
        categoryList[i] = ReadCategories(myFile, header.realmHeaders[i].numCategories, header.realmHeaders[i].numRecipes, header.realmHeaders[i].catListOffset);
    }


    std::cout << "recipe: 0x" << std::hex << recipeList[0][1].itemId << std::endl;
}



// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
