# N8E — duplex 4 MHz après remplacement des fils capteur

**BLOCKED : configuration refusée, aucune acquisition qualifiée.** Les fils capteur ont été remplacés selon l'utilisateur, les fils Adafruit 4682 sont inchangés ; le capteur reste celui emprunté à TR2-001. Les mesures VDD/VDDIO de 3,3 V sont rapportées par l'utilisateur. Aucun changement physique supplémentaire demandé pendant cette tranche.

Code testé : `4307b95cf1f85266266574a04beab96e2fc14b20`. Ajout CMake minimal `R3_SPI3_DIVIDER`, défaut 2 (8 MHz) conservé ; valeur 4 autorisée uniquement pour le profil 8H duplex explicite. Transport duplex préexistant, mode SPI 3. ODR, ±8 g, FIR107 Q15 /10, gain, format v3, importeur et toutes les gardes inchangés. Nouveau préfixe N8E ; aucun fichier créé.

État hôte enregistré pour cette tranche : `PowerOnline=false`, `Discharging=true` ; l’alimentation secteur n’est donc pas acquise pour un futur départ long. COM5 correspond à TR2-002. Ne pas réutiliser une ancienne observation secteur comme état actuel.

## Build, identité et Flash

Build ARM ciblé réussi : text 53012 / data 24 / BSS 549096 octets. Config/build avant commit puis reconstruction de provenance au SHA commité ; pas deux qualifications indépendantes. Avertissements nosys `_close/_lseek/_read/_write` conservés dans les logs. Les anciens tests hôte/canoniques portant sur du code inchangé n'ont pas été rejoués.

BIN 53036 octets, SHA-256 `59e65cd66ecdfe76b7ace5f2ad0affb3c05ed871c804ec2549d11f6228fbc186`. ELF/MAP et options exactes dans `artifact.json` et `arm/`. Avant flash : UID `001B0046/38425018/20333157`, ST-LINK `0032001F3235510C37333439`, N8A entier relu conforme et état sûr. Un seul flash standard à 0x08000000, vérification et reset normal de programmation ; BIN N8E entier relu identique, boot WAIT2. Aucun reset supplémentaire, mass erase, Option Bytes ou firmware ST-LINK modifié.

## Première tentative et premier refus

Une seule commande d'initialisation/calibration 1. WHO=0x7B ; CID/CSD/capacité correspondent à la carte test, montage FAT32 réalisé. Résultat : stage99/error1/run1, FIFO error6 (configuration), stop_error0 ; zéro bracket/anchor/échantillon/sortie. La calibration n'a pas commencé. GDB retourne 6 et `init_card.py` échoue explicitement : logs/traceback préservés. Les contrôles de noms libres et d'espace du script ne sont pas atteints, aucun faux CARD_PREFLIGHT produit.

`CONFIG_TRANSACTIONS.json` décode sans modifier les deux buffers bruts de fin d'initialisation. Transactions déterminantes :

| Index | Accès | Valeur |
|---|---|---|
|26|écriture FIFO_CTRL4 / 0x0A|0x80 (batch timestamps /8)|
|28|écriture CTRL10_C / 0x19|0x20|
|29|lecture FIFO_CTRL4|0x00|
|30|écriture FIFO_CTRL4 en mode continu|0x06|
|33|lecture registres 0x07..0x0A|00 01 0A **06**, attendu 00 01 0A **86**|

Les statuts HAL sont à zéro, ce qui ne prouve pas la validité physique de chaque octet. Contrôle final attendu conservé : aucune correction du résultat lu ou de la valeur attendue pour forcer l'acceptation. Le cleanup se déroule ensuite. Le décodage du driver ST et du constructeur de trame ne montre pas de divergence de valeur demandée : écriture 0x80 effectivement préparée ; cela ne prouve pas son observation sur les broches.

## Contrôle discriminant borné, sans nouvelle calibration

Après vérification ODR off / FIFO bypass / timestamps off à 1 MHz, écriture volatile du seul champ de décimation FIFO_CTRL4 (0x80, FIFO toujours bypass) et lecture croisée 1/4 MHz. Résultats : écriture4/lecture4=0 ; écriture4/lecture1=0 ; écriture1/lecture1=0 ; écriture1/lecture4=0. Huit retours HAL à zéro. Valeur originale FIFO_CTRL4=0 restaurée à 1 MHz puis prescaler et handle remis à **4 MHz**, conformément à N8E. Aucun START FIFO/ODR/timestamps, reset ou retry de calibration pendant ce contrôle. Voir `CONFIG_BIT_CROSSCHECK.json` et le script GDB exact.

La [fiche ST IIS3DWB DS12569 Rev8](https://www.st.com/resource/en/datasheet/iis3dwb.pdf), §9.5/table18, décrit ce champ en lecture/écriture et 0b10 comme décimation8. La table consultée ne fournit pas de condition expliquant ici sa disparition ; cela ne démontre pas l'absence de toute contrainte d'état non identifiée. Le défaut ne peut plus être attribué uniquement à une lecture à 4 MHz. Le transport, l'état capteur et les conditions électriques restent à départager ; aucune cause unique ni défaillance du module initial démontrée.

## État final et reprise

`READY.json` : prepared=false. Dernière vérification GDB : UID exact, stage99/error1/run1, FIFO arrêté, writer inactif, clock_fault0, SD writes0 / failed=false, CS SD haute. GDB détaché, serveur terminé. N8E duplex 4 MHz conservé en Flash, boot WAIT, **non qualifié**. ODR/FIFO/timestamps arrêtés selon lectures et cleanup ; pas de mesure électrique des lignes. Aucun gate SD ni long lancé ; zéro fichier N8E créé. Carte historique non formatée/réparée/supprimée, FRAM/COM4/logiciel TR2-001 hors opérations.

Pour reprendre : préserver ces résultats et lever le défaut de configuration SPI avant toute nouvelle calibration/gate ; un contrôle des trames réelles SCK/CS/MOSI/MISO permettrait de distinguer commande envoyée, réception et relecture. Aucun relevé de forme d'onde disponible ici, aucune cause câblage exclusive annoncée. Un tel contrôle physique nécessite le matériel et l'intervention utilisateur selon AGENTS.md §9. Ne pas empiler des flashes identiques ni ignorer le refus ; ne pas lancer 8h. Après correction démontrée seulement : calibration stricte, gate FIFO+SD borné et relecture complète, puis préparation READY et signal utilisateur LANCE 8H.

Tous les originaux restent dans `/home/lolo/dev/msm/reports/TR2-r3-N8E-duplex4-20261101`. `MANIFEST.json` donne SHA-256 et taille des contenus originaux ; décompresser zlib avant comparaison. Scripts de gate/long archivés pour provenance, **non exécutés** pendant cette tranche.
