#include <iostream>
#include <cstdint>
#include <cstring>

// texture flags are read at 00629466
// ui shouldn't have 0x1000 but ingame should
// set by 625D07 for dds header + 0x24 being & 1
// the 0x1000 flag sets the texture as SRGB

int main(int argc, char *argv[]) {
	if (argc < 2) {
		std::cout << "Usage: FlatOutUCDDSFixer_gcp.exe <filename>";
		return 0;
	}
	auto file = fopen(argv[1], "rb");
	if (!file) return 0;
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);
	auto arr = new uint8_t[size];
	fread(arr, 1, size, file);
	if (size < 4 || ((uint32_t*)arr)[0] != 0x20534444) {
		std::cout << "Not a DDS file";
		fclose(file);
		return 0;
	}
	bool fileChanged = false;
	if (argc > 2 && !strcmp(argv[2], "-fix_header")) {
		if (size > 0x4C && arr[0x4C] == 0x18) {
			arr[0x4C] = 0x20;
			fileChanged = true;
		}
	}
	if (size > 0x24 && (arr[0x24] & 1) == 0) {
		arr[0x24] = 1;
		fileChanged = true;
	}
	if (!fileChanged) {
		std::cout << "File doesn't need patching";
		fclose(file);
		return 0;
	}
	fclose(file);

	if (file = fopen(argv[1], "wb")) {
		fwrite(arr, 1, size, file);
		fclose(file);
		std::cout << "DDS patched";
	}
	delete[] arr;
	return 0;
}
