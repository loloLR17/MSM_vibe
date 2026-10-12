# M015-T — CPU160 et SPI4 indépendant, avant cible

Référence S : CPU16MHz, PCLK3/MSI16/div4, premier burst OVR+timeout et63/896octets consommés. Une seule acquisition nouvelle4096XYZ autorisée, pas de répétition S/2MHz/512 ni8MHz.

Le [STM32U575 DS13737Rev10](https://www.st.com/resource/en/datasheet/stm32u575zi.pdf) admet CPU160MHz et montre HSI16 comme source SPI3 indépendante. CubeU5v1.9.0 confirme RCC_SPI3CLKSOURCE_HSI et HAL_RCCEx_GetPeriphCLKFreq retourne HSI_VALUE16MHz si HSIRDY. Le profil T utilise MSI4MHz/PLL M1 N80 R2 (VCO320/CPU160), AHB/APB1/2/3 div1, PWR VOS1/SMPS, FLASH_LATENCY_4 ; séquence PLL/VOS/latence issue du main.c existant, sans modifier ce fichier primaire. HSI16 est explicitement activé et sélectionné pour SPI3, div4 conservé, SCK4MHz calculés indépendamment de PCLK3=160MHz. I/O3,3V utilisateur antérieur et limite IIS3DWB10MHz inchangées.

Différences nécessaires : fréquence CPU/APB, VOS/latence Flash, activation PLL/HSI et sourceSPI3. Source S MSI contre source T HSI : ni fréquence physique ni égalité exacte des oscillateurs calibrées ici. Ne pas attribuer un éventuel succès au seul compteur CPU en excluant source/latence/APB. Aucun changement mode3/ODR/vendor/parser/buffers/journal/instrumentation/guards/cleanup. DWT brut inchangé, conversion hors ligne160cycles/µs au lieu16. Profil par défaut16 conserve byte-identique BIN S4 ; option160 exige div4, autres couples rejetés.

Avant trigger : UIDsecond/SN/Flash/BIN/ELF identifiés, verify et readback exact, dernier CLI puisresetGDB, ready1/faults0. Contrôler bench_core_hz et SystemCoreClock160000000, RCC_CR.HSIRDY, CCIPR3.SPI3SEL=HSI(10h), CFGR3.PPRE3 div1, PWR_VOSR.VOS=1 (bits30000h)/VOSRDY8000h et FLASH_ACR.latency4, SPI_CFG1.MBR=div4 (10000000h). Sauvegarder RCC/PWR/FLASH_ACR après capture. Aucun appel GDB de fonction target nécessaire, lectures de registres uniquement.

Critères Q/S conservés :4096XYZ exacts avecTS séparés, tags/parité/compteur, huitXYZentreTS/monotonie, saturation, statuts FIFO FULL/OVR/latch/niveau, HAL/error0 etRXsize/remaining/offset/CS complets avantclose, reconstruction byte-identique. Réservescleanup12/32/24,500ms/400statuts, buffers inchangés. STOP tout refus ; capteurau repos XLoff/bypass/TSoff, faults0/detach. Aucune perte globale zéro ou validationduTR2primaire présumée.

Tests nouveaux proportionnés :deux refus CMake (core80 et160/div8), deuxARM (S16 témoin compile uniquement/BINidentique etT160). Les tests duparser etlescampagnesphysiques précédentes sont inchangés et ne sontpasrejoués. Il ne s'agit pas de qualificationphysique avantcapture.
