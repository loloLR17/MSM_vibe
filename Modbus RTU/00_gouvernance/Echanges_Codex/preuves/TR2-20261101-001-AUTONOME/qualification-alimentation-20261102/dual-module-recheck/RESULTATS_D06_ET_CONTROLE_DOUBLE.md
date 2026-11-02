# Contrôle A/002 et B/001, puis D06 sur001

## Conditions et inventaire

Utilisateur signale une correction (nature précise non rapportée), module A sur002,
module B sur001, deux bancs connectés ST-LINK. Le transfert deA vers002 hors tension
n'est pas explicitement reconfirmé dans ce message ; ne pas le présenter comme une
mesure instrumentale. Pas de nouvelle tension rapportée. Aucun recâblage agent.

SN/UID des deux bancs vérifiés séparément. BIN001 D01 sourceacb14279/50912B et
BIN002 N8G sourcefbbdde5d/53032B intégralement relus identiques aux images archivées.
Les gros fichiers restent localementT1000. FIFO/writer arrêtés et SD0 sur les deux.
001 conserve initialement l'ancien99/error1/WHO0 ;002 est auWAIT2/run0/error0,
WHO0nonencoreluparsonfirmware. Ces variables ne sont pas les lectures actuelles.
Premier script d'inventaire interrompu sur symbole r3_command absent du BIN001
lié (inutilisé dans ce profil) ; corrigé en retirant cette seule inspection GDB.
Les scripts et logs sont préservés ; pas un échec du firmware ni un reset.

## Lectures actuelles, sans configuration capteur

Appels de lecture ST qualifiés viaGDB, gardeFIFO/writerSDinactifs, scratchRAM4B
sauvé/restauré. Deux lectures par cible : WHO0F et CTRL1_XL10. Pas de registre écrit.
**B/001 WHO7B, CTRL100 ; A/002 WHO7B, CTRL100**, HAL0/tracescomplètes.
La liaison répond sur les deux ; aucune qualification FIFO/durée du module A sur002.
Aucune modification du firmware, commandeSD/FRAM/COM4, flash ni reset002.

## Réarmement001 justifié et unique acquisition D06

Le défautWHO0 est verrouillé dans l'ancien démarrage001 alors que la lecture
actuelle donne7B. Un seul resetlogiciel001, après vérification arrêt/SD0, nécessaire
pour reprendre la calibration avec le montage corrigé. Aucun flash/build.
Calibration001 PASS :198brackets/6574ancres, résidumax24µs≤100, FSreadback04/±16g,
WHO7B, SPI3 8MHz, WAIT2/run1/error0. Observateur armé auWAIT ; USERpressé/relâché
confirméuser, retourrouge. Un seul essai prévu60s, aucune répétition aveugle.

Résultat **3,010048sMCU,99/error3**,31checks/79256natifs/7926sorties calculées,
0rails/clips/flags/erreurFIFO, STOPpropre134µs. Incohérence du compteur direct :
raw17571384 reçu dans les octets **38 1E 0C 01**, registre40/4octets,
HALstatus/error0/RXTXrestants0/valid1. DernierFIFO397142, précédentdirect389251,
bracket73530397–73530415µs. Mapperbase73429014/span100226/7946ticks/ticks7968 :
prédiction290154936/résidu216624521µs, recalcul exact/limite100 inchangée.
Budgetmask0 ; maxima polls complets692,96875/373,625/951/1,0125µs (dernierappel
writer vide, pas une écriture). Pas de mesure des budgets d'un poll interrompu
avant comptabilisation ; pas de cause électrique exclusive inférée du HAL.

Snapshot67488B CRCdb4f4cc4 conforme/sourceacb14279/SHA
1898379fcb1222e56a44817398f8e4397b56d085d07766ef2f5cb568aa621ea1.
Trace fautive préservée avant tout reset supplémentaire. SDcommands/reads/writes0,
initialized/write_enabledfalse, writerinactive, FIFOstoppedtrue, CS SDhaute,
GDBdétaché. Pas de fichierSD, tousXYZnonpersistés : snapshots/digests seulement.
002 laissé inchangé après lectures, N8GWAIT sans commandeSD/START.

## Conclusion et suite

La correction rétablit WHO sur001 et permet une calibration, mais l'acquisition
échoue encore avecB sur001 alors queB avait passé2×60s sur002. Le défaut n'est pas
expliqué par le seul moduleA ni par la SD/sontrafic. Capteur/liaison/contacts/
alimentation/Nucleo/interactionlogicielle restent à départager, pas de MCUcondamné.
La cause électrique n'est pas mesurée. Ne pas présenter002+A comme requalifié
60s/endurance : seules deux lectures actuelles y sont réalisées.

Proposition : préserver complètement les fils002 ; remplacer uniquement les fils
capteur001 par un jeu court du stockutilisateur, surimplantationB actuelle, sans
modifier SD/jumpers/logiciel/fréquence/garde100µs. Geste physique hors tension,
encore non réalisé ; ne pas répéter un essai inchangé en attendant. Nature exacte
de l'erreur de câblage corrigée à préciser si disponible. Aucun longSTART/8hREADY.
