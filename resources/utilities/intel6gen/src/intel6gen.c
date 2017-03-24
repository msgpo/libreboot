/*
 *  Copyright (C) 2014, 2015 Leah Rowe <info@minifree.org>
 *  Copyright (C) 2016 Swift Geek <swiftgeek@gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
/* Generate deblobbed descriptor and gbe 12KiB file from scratch
 * without relying on a factory.rom dump */
 
#include "intel6gen.h"

int main(int argc, char *argv[]) 
{
	int i, j;
	
	struct GBEREGIONRECORD_8K gbeStruct8k = generatedGbeStruct8k();
	struct DESCRIPTORREGIONRECORD descriptorStruct8M = generatedDescriptorStruct(ROMSIZE_8MB);
	
	/* Only for the compatibility checks */
	struct DESCRIPTORREGIONRECORD dummyDescriptorStruct;
	struct GBEREGIONRECORD_8K dummyGbeStruct8k;
	
	/*
	 * ------------------------------------------------------------------
	 * Compatibility checks. This version of intel6deblob is not yet portable.
	 * ------------------------------------------------------------------
	 */

	if (systemOrCompilerIncompatible(dummyDescriptorStruct, dummyGbeStruct8k)) return 1;
	/* If true, fail with error message */
	
	/*
	 * ------------------------------------------------------------------
	 * Arguments given on the terminal
	 * ------------------------------------------------------------------
	 */
	 

	if(argc==3) {
		
		/* If user provides their own MAC address, it will be used.
		 * Otherwise, intel6gen will simply use the default one. 
		 * 
		 * However, if the user provides an invalid MAC address, then intel6gen
		 * will exit. */
		if(0==strcmp(argv[1],"--macaddress")) {
			/* 6 hex chars format (example): AA:BB:CC:DD:EE:FF */
			if (strlen(argv[2]) != 17) {
				printf("intel6gen: invalid mac address format (wrong length)\n");
				return 1;
			}
			for(i=2; i<14; i+=3) {
				if(argv[2][i]!=':') {
					printf("intel6gen: invalid mac address format (non-colon characters used as spacing)\n");
					return 1;
				}
			}
			for(i=0; i<6; i++) {
				gbeStruct8k.main.macAddress[i] = 0;
				
				/* Go through each nibble of the byte */
				for(j=0; j<2; j++) {
					if(argv[2][(i*3)+j]>='a' && argv[2][(i*3)+j]<='f')
						gbeStruct8k.main.macAddress[i] |= (uint8_t)((argv[2][(i*3)+j] - 87) << ((j^1) << 2));
					else if(argv[2][(i*3)+j]>='A' && argv[2][(i*3)+j]<='F')
						gbeStruct8k.main.macAddress[i] |= (uint8_t)((argv[2][(i*3)+j] - 55) << ((j^1) << 2));
					else if(argv[2][(i*3)+j]>='0' && argv[2][(i*3)+j]<='9')
						gbeStruct8k.main.macAddress[i] |= (uint8_t)((argv[2][(i*3)+j] - 48) << ((j^1) << 2));
					else {
						printf("intel6gen: invalid mac address format (non-hex characters)\n");
						return 1;
					}
				}
			}

			gbeStruct8k.main.checkSum = gbeGetChecksumFrom4kStruct(gbeStruct8k.main, GBECHECKSUMTOTAL); /* Fix the checksum */
			memcpy(&gbeStruct8k.backup, &gbeStruct8k.main, GBEREGIONSIZE_4K); /* Copy to the backup */
			
			/* Generate intel6gen data (C code for Gbe region): */

			/* mkgbe.h */
			if (notCreatedHFileForGbeCFile("mkgbe.h", "mkgbe.c")) {
				return 1;
			} /* and now mkgbe.c */
			if (notCreatedCFileFromGbeStruct4k(gbeStruct8k.backup, "mkgbe.c", "mkgbe.h")) {
				return 1;
			}
			
			printf("You selected to change the MAC address in the Gbe section. This has been done.\n\n");
			
			printf("The modified gbe region has also been dumped as src files: mkgbe.c, mkgbe.h\n");
			printf("To use these in intel6gen, place them in src/intel6gen/ and re-build intel6gen.\n\n");
		}
		
	}
	
	/*
	 * ------------------------------------------------------------------
	 * Generate the 12KiB files, ready to be used in a libreboot image
	 * ------------------------------------------------------------------
	 */
	
	if (notCreatedDescriptorGbeFile(descriptorStruct8M, gbeStruct8k, "intel6fdgbe_8m.bin")) {
		return 1;
	}
	
	/*
	 * ------------------------------------------------------------------
	 * Generate the 4KiB files (descriptors without GbE), ready to be used in a libreboot image
	 * In these descriptors, the onboard Intel GbE NIC is disabled; a discrete one is used instead
	 * ------------------------------------------------------------------
	 */
	

	return 0;
}
