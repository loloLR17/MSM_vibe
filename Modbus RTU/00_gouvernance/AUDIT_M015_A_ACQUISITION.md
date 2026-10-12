# M015-A — Audit du chemin IIS3DWB et diagnostic borné

## Chemin existant et responsabilités

Le capteur utilise SPI3, PC10/PC11/PC12 AF6, CS logiciel PC9. Le pinout H3g-B documente l'alimentation 3V3 et l'absence de raccordement INT1/INT2 ; aucune nouvelle mesure d'alimentation ou hypothèse de câblage n'est ajoutée. SPI1/CS PD14 appartient à la FRAM. Le diagnostic ne l'initialise pas et ne démarre ni UART, ni microSD, ni DMA ou IRQ capteur. Un seul client synchrone détient SPI3 ; SysTick reste actif pour les délais HAL.

Le driver `stm32_iis3dwb_vibration_source.c` est réutilisé : WHO_AM_I 0x7B, CTRL1_XL 0xA0 et bits d'échelle 0/8/C/4, CTRL3_C 0x44 pour BDU et IF_INC, STATUS 0x1E/XLDA, burst de six octets depuis 0x28. Les sensibilités nominales sont 61/122/244/488 µg/LSB ; conversion entière symétrique en mg. Le masque d'axes est logiciel ; CTRL6=0 est vérifié pour les trois axes du capteur.

`configure` active immédiatement le capteur ; `start` et `stop` sont des états logiciels. Le diagnostic sauvegarde puis restaure explicitement CTRL1/CTRL3/CTRL5, même après une erreur partielle. La nouvelle lecture brute et la conversion production partagent le même code ; les sorties sont effacées sur absence DRDY ou erreur. Les écritures de diagnostic sont limitées à ces trois registres volatils, avec refus des resets/reboots et du mode d'auto-test interdit.

## Dépendance normative et voie autorisée

Le bloc 4 V1, §8, impose fréquence 26667, masque d'axes 1..7, échelle 0..3, stockage local mode 1 et limite positive compatible avec la capacité utilisable. `configuration_store` exige un environnement disponible ; `command_start_acquisition` refuse sans snapshot actif et `acquisition_service.begin_window` exige ce snapshot.

La configuration FRAM M014 est structurellement valide mais opérationnellement UNAVAILABLE faute de stockage disponible. Cette règle est conservée : aucun snapshot actif, READY, STORAGE_AVAILABLE, backend fictif ou commande B5. Le contrat M015-A autorise alors un diagnostic d'acquisition isolé, borné et sans écriture persistante. Il ne constitue pas une acquisition de campagne.

## Diagnostic et contrôles

L'option `TR2_STM32_ACQUISITION_CAPTURE` est OFF par défaut et exclusive de production, manufacturing, probe, diagnostic Modbus, TX et observer. Son entrée précède le harness. Le trigger RAM unique 0x4D303135 lance quatre phases de 512 échantillons à ±4 g : repos, auto-test positif, négatif et retour au repos. Chaque phase attend 100 ms et jette la première lecture prête. Aucun debugger n'est présent pendant la capture.

Les registres FIFO, routes d'interruptions, axes, filtre et offsets sont audités ; une configuration non attendue est refusée. WHO_AM_I et les registres modifiés sont relus. Le polling est limité par un nombre fini de samples et une échéance de 100 ms par sample. CS est relâché, les erreurs sont remontées et la restauration est tentée aussi après échec. Le profil s'arrête en boucle d'attente et ne peut pas tomber dans un harness de stockage.

DWT CYCCNT est vérifié en progression ; HCLK et la fréquence SPI sont enregistrés. Les timestamps encadrent STATUS + burst XYZ et ne sont pas des instants de conversion ni de l'UTC. BDU protège les mots LSB/MSB, mais la simultanéité des conversions XYZ n'est pas mesurée. FIFO et DRDY physique ne sont pas utilisés.

## Comparaison physique et conclusion

Trois captures mode 0 donnent une distribution bimodale proche d'un facteur deux, malgré HAL_OK : TX/RX séparés à 2,5 MHz, duplex unique à 2,5 MHz, puis duplex à 625 kHz. Elles présentent 1/3/6 saturations et une norme au repos d'environ 1,53 g. Les données restent intégralement publiées.

Le HAL U5 ferme le périphérique entre TX et RX séparés, ce qui motivait une hypothèse de décalage de bit. Cette hypothèse n'est pas confirmée : une transaction duplex unique n'a pas suffi. La vitesse seule n'a pas suffi non plus.

À 625 kHz, le seul changement CPOL haut/CPHA deuxième front vers le mode 3 supporté par ST fournit 2 048 samples cohérents, zéro erreur/saturation, un vecteur au repos proche de −157/+52/−971 mg et des écarts d'auto-test de 1,6 à 1,9 g par axe. Le retour au repos est observé. La configuration fonctionne dans cet essai ; la cause électrique précise reste non mesurée sans analyse SCK/MOSI/MISO.

La cadence de lecture est d'environ 6 538/s, pour un ODR configuré à 26,667 kHz, non mesuré indépendamment. La transaction dure environ 151 µs. Des échantillons intermédiaires peuvent être perdus : pas de flux continu, FFT, bande utile de 6 kHz ou métrologie qualifiés.

## Références et préservation

Sources primaires consultées le 12/10/2026 : [ST DS12569 Rev 8](https://www.st.com/resource/en/datasheet/iis3dwb.pdf), notamment §§2.1, 3.4 et 9.11..9.15, et [exemple officiel d'auto-test ST](https://raw.githubusercontent.com/STMicroelectronics/STMems_Standard_C_drivers/master/iis3dwb_STdC/examples/iis3dwb_self_test.c). Le mode 3 est supporté ; l'auto-test mécanique est prévu à une échelle au moins égale à ±4 g, avec stabilisation et limites de 800 à 3200 mg. Cette excitation interne ne qualifie pas une vibration externe ni une calibration.

H3g-C historique contient une observation de changement d'orientation et une contradiction éditoriale entre statut validé/action encore attendue. Elle est signalée, non corrigée hors mission et non présentée comme une nouvelle preuve M015.

Après chaque diagnostic, le BIN permanent M014-H, SHA b462c29a775fec17fccf8036b5612742b1366210359bcd3f192c95435740d611, est restauré par la chaîne NORMAL qualifiée, avec relecture complète Flash 2 Mio identique au backup initial. Jamais HOTPLUG, mass erase ou Option Bytes. Les registres du capteur sont restaurés à 0/4/0 ; le boot retrouve FRAM V2/génération 12/configuration UNAVAILABLE et IRQ attendues. Le contrôle physique B0/B1 final réussit.

Le permanent M014 conserve le mode 0 et n'acquiert pas. La nouvelle acquisition mode 3 est uniquement démontrée dans le profil RAM ; son raccordement production, FIFO/DMA, timestamps de conversion et stockage relèvent d'un contrat suivant. Aucun accès microSD, provisioning FRAM ou changement des gels V1 ; partition LBA65536 préservée.
