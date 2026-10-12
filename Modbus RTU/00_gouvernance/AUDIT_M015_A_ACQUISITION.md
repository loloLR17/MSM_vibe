# M015-A — Audit et diagnostic acquisition

## Chemin réel et frontière normative

SPI3 exclusivement polling, PC10/11/12 AF6 et CS PC9, mode0/prescaler64 ; aucun IRQ capteur, DMA ni INT1/INT2 raccordé selon le pinout H3g-B. Alimentation 3V3 du banc documentée historiquement, aucune nouvelle mesure de tension capteur. SPI1/PD14 appartient à FRAM et n'est pas initialisé dans le profil isolé. Un seul opérateur synchrone détient SPI3. SysTick HAL reste actif ; aucun IRQ SPI ni transport n'est démarré.

Driver existant `stm32_iis3dwb_vibration_source.c` réutilisé : WHO0x7B, CTRL1_XL ODR0xA0 et FS0/8/C/4, CTRL3_C0x44 BDU+IF_INC, STATUS0x1E/XLDA, burst6octets depuis0x28. Sensibilités nominales61/122/244/488µg/LSB, arrondi entier symétrique enmg. Masque axes logiciel ; sensorXYZ vérifié CTRL6=0. Le driver configure active immédiatement ODR ; start/stop sont des états logiciels, pas un arrêt électrique. Le diagnostic sauvegarde/restaure explicitement CTRL1/3/5. Sa conversion brute et production est désormais commune ; échantillon effacé/invalide en cas d'erreur ou absence DRDY.

`bloc4.md` §8 : sampling26667 uniquement, axes1..7, FS0..3, stockage1 et limitepositive obligatoires. `configuration_store` exige environnement disponible ; `command_start_acquisition` refuse sans snapshot actif ; `acquisition_service.begin_window` exige ce snapshot. FRAM gen12 M014 demeure structurellement valide mais opérationnellement UNAVAILABLE sans stockage. Aucun contournement READY/STORAGE, commandeB5 ou écriture persistante. Diagnostic isolé permis expressément par contratM015-A, pas acquisition de campagne.

## Procédure bornée

OptionOFF `TR2_STM32_ACQUISITION_CAPTURE`, exclusive de production/manufacturing/probe/diagnostic/TX/observer. Entrée avant harness ; aucun média ni UART. TriggerRAM unique0x4D303135 ; 4×512 échantillons, ±4g et26.667kHz configurés, readbacks obligatoires. Phases repos/auto-testpositif/négatif/repos,100ms stabilisation et premièrelecture jetée à chaquephase. RegistresFIFO/routes/axes/filtre/offsets audités et paramètres non défaut refusés ; trois registres modifiés volatils seulement et restaurés même après erreur. Deadline100ms parsample et nombre fini. Pas de reset capteur ni modification persistante.

DWT CYCCNT vérifié en progression, fréquenceHCLK enregistrée ; begin/end bornent la transactionSTATUS+burst. Ce sont des timestamps MCU de lecture, pas l'instant physique de conversion, pas UTC. Aucun debugger pendant la capture. Pollingprescaler64 ne garantit pas26667échantillons/s : fréquence de lecture mesurée, FIFO non utilisé, pertes d'échantillons intermédiaires possibles. Aucun taux ODR mesuré indépendamment sansDRDY/FIFO ; ne confondre fréquenceconfigurée et cadenceobservée.

## Références primaires et limites

[ST DS12569 Rev8](https://www.st.com/resource/en/datasheet/iis3dwb.pdf), §§2.1/3.4/9.11..9.15 ; [exemple officiel auto-testST](https://raw.githubusercontent.com/STMicroelectronics/STMems_Standard_C_drivers/master/iis3dwb_STdC/examples/iis3dwb_self_test.c), consultés12/10/2026. Auto-test interne mécanique prévu ≥4g, attente100ms, écart800..3200mg paraxe. Il ne qualifie pas une vibration externe contrôlée ni une calibration. H3g-C historique prouve une réponse à orientation ; contradiction éditoriale dans ce document (statutvalidé suivi d'une actionencoreattendue) signalée, pas corrigée hors périmètre ni nouvellepreuveM015.

## Préservation matérielle

Firmware permanentM014 BIN b462c29a775fec17fccf8036b5612742b1366210359bcd3f192c95435740d611 conservé avecELF etbackupsH. Avantflash : identitéST-LINK/UID, nouveauFlash/RAMdump, comparaison du préfixe permanent. FlashNORMAL 0x08000000 seulement, vérification ; resetGDB aprèsCLI, jamaisHOTPLUG. Restaurer ce BIN exact aprèsdiagnostic et vérifierreadback/bootFRAMgen12, configurationUNAVAILABLE, IRQ etabsencefaute. ContrôleB0/B1 borné si nécessaire pour constaterreprise aprèsflash. Aucun accèsmicroSD, aucuneFRAMwrite/provisioning. Preuvesphysiques à obtenir : aucun succès annoncé sur seules compilations.
