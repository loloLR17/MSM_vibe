# Bring-up RX RS-485 — mission 008, 11 octobre 2026

**COMPILÉ STM32 / NON FLASHÉ / NON QUALIFIÉ RX.** Préparation logicielle achevée ; essai physique conditionné aux contrôles et permissions ci-dessous. Les freezes, interfaces, serveur sans réponse et dette microSD E4 restent inchangés.

## Faits et schéma

Observations utilisateur reprises du contrat : logique 3,31 V ; SB28 fermé, 3,31 V aux deux extrémités ; SB29 ouvert (3,3 V / 0 V). DE câblé PG4, /RE et PG5 sans fil externe. Pont J2 soudé rapporté, révision réelle non identifiée. PG4 mesuré 0,88 V au repos ; flottement possible, non démontré. GDB fermé selon utilisateur ; OpenOCD peut rester actif ; microSD absente. Firmware actuellement en Flash différent du BIN006, non identifié.

Lecture graphique du [schéma officiel v102](https://download.mikroe.com/documents/add-on-boards/click/rs485_lsolator_2_click/rs485-isolator-2-click-schematic-v102.pdf) : **J2 relie DE à /RE ; J3 relie /RE à RST**. La [table fabricant](https://www.mikroe.com/rs485-isolator-2-click) appelle JP2 le contrôle commun et J3 le contrôle indépendant. Cette concordance fonctionnelle ne vérifie ni la révision ni la continuité du pont réel. J2 effectivement fermé implique DE bas et /RE bas sans fil PG5 ; vérifier que RST n'est pas commandé par ailleurs via J3.

Le schéma v102 relie **VIO (U1 pin7) à VCC sélectionnée (pin4)** : une sélection 5 V alimente aussi VIO en 5 V. Les mesures SB28 ne démontrent pas la tension du Click. Vérifier VCC/VIO proche de 3,31 V côté logique GND1 avant essai ; préserver l'isolation GND1/GND2. INVD/INVR ont des résistances vers GND1 dans ce schéma. Le [datasheet ADM2867E, table12](https://www.analog.com/media/en/technical-documentation/data-sheets/adm2867e_adm2865e_adm2863e_adm2861e.pdf) confirme DE bas = émetteur haute impédance et /RE bas = réception activée. 0,88 V peut être inférieur au seuil VIL nominal (0,3 VIO), mais une mesure isolée ne garantit pas un niveau bas stable.

## Séquence réellement compilée

`HAL_Init → HAL_MspInit → horloges PWR/GPIOG → BSRR PG4 bas → PG4 output push-pull/pull-down/vitesse faible → HAL_PWREx_EnableVddIO2 → plus tard PG7/PG8 AF8 et LPUART1`.

HAL officielle STM32CubeU5 v1.9.0 : WritePin RESET précharge le latch avant le mode output ; EnableVddIO2 positionne IO2SV. Initialisation placée au début du boot, avant les qualifications FRAM/capteur/SD et avant UART. Aucun PG5, aucun DE haut, aucune émission ajoutée. Recherche des écritures GPIOG/IO2SV : seul cet initialiseur configure PG4/IO2SV ; le transport configure ensuite PG7 TX / PG8 RX AF8, 115200 8E1.

[AN5373 §2.1.2](https://www.st.com/resource/en/application_note/an5373-getting-started-with-stm32u5-mcu-hardware-development-stmicroelectronics.pdf) : IO2SV libère tout PG2..PG15, après présence effective du rail. Présence ici fondée sur les observations du contrat, pas une mesure automatique HAL. Aucun autre utilisateur de ces pins ajouté ; leur état électrique après libération n'est pas mesuré. Au reset et avant HAL_MspInit, la sortie peut rester isolée/flottante : le pull-down logiciel ne garantit pas cet intervalle. Une garantie matérielle permanente nécessiterait une décision et une qualification séparées.

Test hôte du vrai source avec HAL simulée : latch initial haut, ordre strict, absence d'écriture haute, PG5/autres bits préservés, IO2SV initialement actif/inactif. Vérification objdump de l'ordre des appels dans l'ELF ARM. Ces preuves n'observent ni impulsions ni niveaux électriques.

## Un seul essai futur, après autorisations distinctes

1. Utilisateur : identifier révision et continuité réelle J2 DE–/RE, absence de commande RST concurrente ; vérifier VCC/VIO ~3,31 V côté GND1. Ne pas modifier ponts ni ajouter PG5 dans cette procédure. Coordonner propriétaire exclusif ST-LINK : GDB fermé ne prouve pas OpenOCD arrêté. Conserver microSD absente.
2. Après permission explicite de flash, utiliser la procédure Windows ST qualifiée du projet, ST-LINK `003500463235510B37333439`, adresse `0x08000000`, sauvegarde et vérification prévues. Flash WSL/OpenOCD non qualifié. BIN008 157944 octets, SHA256 `02ed2cf64d4d78225a4781b823c9d27e96a8ea693679d8a834d230568383593b`. ELF008 SHA256 `6546629b85dba6ba5ead029d208d9a95e67a13e0e244d477d01828b07ab4c23e`. Artefacts locaux `05_Firmware/build-stm32-p11c/tr2_stm32_p11c.{bin,elf}` ; vérifier les empreintes avant utilisation. Aucune de ces opérations n'a été exécutée en008.
3. Après permission de debug, identifier Flash contre BIN008 avant tout usage de symboles ELF008. Vérifier IO2SV actif et mesurer PG4/DE et /RE franchement bas après boot. Si pont/révision/tensions ou identité Flash ne concordent pas, arrêter l'essai.
4. Suivre la [procédure006](PREPARATION_RS485_RX_006.md) pour script HP et snapshots, en remplaçant impérativement les empreintes006 par008. Snapshot AVANT avec RX ready=1, reprendre cible, envoyer **une fois** COM7 115200 8E1 `01 03 03 E8 00 01 04 7A`, attendre silence/traitement (1 s), snapshot APRÈS. Attendus : +8 octets, +1 ADU valide exacte, aucune erreur UART/CRC/overflow ; aucune réponse attendue. Ne pas suspendre pendant la réception/T3.5. Conserver observations et deltas, sans reset des compteurs.

Cette procédure est PRÉPARÉE ; réception et sûreté électrique restent NON VÉRIFIÉES.
