# M015-D — Diagnostic discriminant de lecture FIFO active

## Acquis réutilisés

B : 171 statuts, 11 reculs avant toute lecture de données, premier127→0 ; FIFO_CTRL2 vérifié81 puis relu80 ; overrun brut non qualifié. C :512 lectures cohérentes au repos (WHO7B, CTRL3=04, FIFO vide), mode3/625kHz, même framing. Ces campagnes et leurs tests inchangés ne sont pas rejoués. Les motifs bit7/décalage sont des hypothèses ; aucune donnée n’est corrigée.

[AN5444 §§4.4/6](https://www.st.com/resource/en/application_note/an5444-iis3dwb-ultrawide-bandwidth-lownoise-3axis-digital-vibration-sensor-stmicroelectronics.pdf) et [DS12569 §§3.2/4.3/9](https://www.st.com/resource/en/datasheet/iis3dwb.pdf) : R/W bit7, adresse7bits, auto-incrément IF_INC, commande puis donnée sans dummy supplémentaire, CS bas pendant tout le transfert. BDU impose3A→3B ; niveau10bits. FIFO_CTRL2=81 limite le modeFIFO1 au seuil256. Bypass remet la FIFO à zéro. Le cas327/flags n’est pas déduit d’une lecture incohérente. Le code respecte masque03FF et ordre ; pas de nouveau changement mode/fréquence.

## Test unique, sans vidange

| Hypothèse | Discriminant | Limite |
|---|---|---|
| Lecture courte/burst incohérente | Comparer statuts3A/3B groupés et deux singles successifs, CTRL2 single et bloc07..0A | Pendant remplissage, tenir compte du temps entre lectures ; égalité exacte non requise |
| Problème propre au compteur dynamique/BDU | Couper CTRL1 après remplissage, garder FIFO mode1, comparer32 groupes sans DATA_OUT | Stabilisation2ms ; pas preuve de conversion ou consécutivité XYZ |
| Corruption des octets hauts/configuration | Journaliser WHO7B, CTRL3=44, CTRL2=81 en alternance avec statuts | Les readbacks seuls ne mesurent pas le signal électrique |
| Défaut de CS/flags HAL | TX/RX bruts, marqueursODR CS, DWT, SRavant/après, CFG et compteursHAL | Pas capture wire ou flags internes continus ; analyseur logique si nécessaire |

Un seul profil dédié OFF/exclusif, trigger4D313544. Sept contrôles volatils sauvegardés, état préexistant actif refusé ; configurationmodeFIFO1/STOP_ON_WTM256/BDR26.667/±4g/BDU. Après100ms de stabilisation capteur en bypass, activationFIFO puis groupes7lectures pendant15ms maximum/borne700records, arrêt surWM lu. Coupure entrée CTRL1 sans vider,32groupes figés puis restauration des7contrôles. Borne totale1024×80octets ; réserves pour restauration. Pas acquisition continue/overrun volontaire/lectureFIFO_DATA_OUT/XYZ/FRAM/SD/UART.

Le statut physique et la cause ne sont pas déduits de HAL_OK. Une fiabilité active exige statuts cohérents, valeurs constantes attendues, progression compatible avec temps, et stabilisation sans reculs inexpliqués. Une anomalie de constante exclut la seule dynamique du compteur ; byte correct sur fil mais faux enRAM permettrait isoler réception/HAL. Sans discrimination concluante, BLOCKED.

## Validation ciblée

Test hôte neuf : nominal, anomalie bit7 conservée dans la trace, mismatch configuration refusé avec restauration, capteur préexistant actif refusé sans écriture ; aucun accèsDATA_OUT, une seule activationFIFO, CS et RXcommande conservés. Première simulation ratée : le niveau128 tombait sur lecture simple et pas sur burst prévu ; injection au premier burst >=128 et vérification par index exact corrigées, sans affaiblir les critères. Test final1/1 et cross-build défaut/profil réussis. Cœur/driver et testsA/B/C/.NET inchangés nonrejoués ; warningsNewlib conservés.
