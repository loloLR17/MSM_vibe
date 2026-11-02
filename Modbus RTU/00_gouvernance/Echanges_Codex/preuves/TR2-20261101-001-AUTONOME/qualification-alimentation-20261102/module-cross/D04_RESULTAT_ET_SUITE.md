# D04 — Module B transféré de TR2-002 vers TR2-001

## Conditions rapportées par l'utilisateur

Les deux bancs ont été mis hors tension ; module A initial de001 repéré et mis
à part ; module B provenant de002 monté sur001, câblage propre à001 conservé.
Remise sous ST-LINK seul demandée sur001, JP6 STLK/JP2 retiré/CN15 débranché,
lecteur001 vide et512Go auPC,002 maintenu hors tension ; utilisateur rapporte
**VDD=VDDIO=3,315V**. Les tensions sont des mesures utilisateur, pas une capture
électrique automatisée. Aucun autre échange de capteur ou fil n'est effectué.
Le module B est celui des deux essais D02/D03 réussis de60s sur002.

## Contrôle réel sans reset ni flash

Cible SN003500463235510B37333439/UID004A003D/38425018/20333157.
Flash D01 lue intégralement sur50912B : strictement identique au BIN conservé,
SHA6a00ec329c3b16cbf1c01fc3661014ad6f1fed05732a60fecbc338d6a7bb98d2,
sourceacb142794570ded5abd15f6521eb5d8b644590e8. Pas de compilation ni de nouveau
flash/reset/commandeSTART ; les anciens tests réussis ne sont pas répétés.

WHO_AM_I=0x7B, FSreadback0x04 (±16g), SPI3CFG1=0x00070007/8MHz.
Le firmware est déjà au refus de calibration **stage99/error7/run0**, avant WAIT.
198 brackets/6574ancres FIFO ; FIFOerror0/stoppedtrue/stop_error0,
clock_gap_fault0. Résidu maximal **213382894µs >100µs**.
Le précontrôle GDB quitte avec code11 parce que la garde WAIT est refusée ;
ce code est la conséquence attendue de l'état observé, pas un succès de qualification.
Pas de60s ni d'appuiUSER demandé. Second relevé de préservation détaché proprement.
SDcommands/reads/writes=0, initialized/write_enabled=false, writerinactif,
CS SD haute. Aucun accès FRAM/COM4/002, formatage ou manipulation de protection.

## Recalcul hors ligne

Les198 brackets sont sauvegardés ; structure24B, avant/aprèsu64 etcompteuru32.
Reprise exacte de calibrate() : bracket10 de base102595µs ; span1892949µs,
150070ticks jusqu'au dernier bracket. Quatre lectures hors garde aux indices
136/153/173/174 ; toutes les valeurs et résidus sont dans D04-analysis.json.
Maximum recalculé213382894µs identique au résultat embarqué.
Par exemple index136 : raw16929951 alors que les voisins valent108403/110008.
Les chaînes hexadécimales du JSON sont des **réencodages des entiers sauvegardés**,
pas les octets SPI originaux : calibrate() réinitialise la trace avant STOP.
La trace SPI finale ne permet donc pas de reconstruire les transactions fautives
originales ; aucune prétention de cause électrique exclusive.

## Déduction et suite proposée

Le module B passe sur002 puis rencontre une incohérence temporelle dès calibration
sur001. L'hypothèse « seul le module A serait défectueux » ne suffit donc pas à
expliquer les observations. Le remplacement du module seul ne résout pas001.
Les fils/contacts, alimentation effective et Nucleo001 restent à départager ;
une interaction logicielle dépendante des conditions n'est pas formellement exclue.
Pas de preuve de MCU001défectueux ; pas d'essai d'écritureSD dans D04, le refusSD
historique reste distinct et nonrésolu.

Prochain changement discriminant proposé : garder le module B et utiliser sur001
le jeu de fils capteur qui a servi aux deux succès sur002, aux mêmes fonctions
SPI3/3,3V/GND, sans toucher au lecteurSD ni aux jumpers. Toutes les manipulations
hors tension, utilisateur seul opérateur physique. Ne pas modifier la fréquence,
la garde100µs ou le logiciel en même temps. Ce changement n'est pas encore réalisé.

État final : 001 arrêté au refus de calibration, FIFO/writeroff, SD0/CShaute,
GDBdétaché ;002 noncontacté dans cette tranche, dernièreFlashN8G restaurée inchangée.
