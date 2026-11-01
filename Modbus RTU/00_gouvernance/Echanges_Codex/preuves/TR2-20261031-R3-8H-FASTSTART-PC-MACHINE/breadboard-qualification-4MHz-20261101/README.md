# Nouveau montage breadboard — reprise fonctionnelle à 4MHz, réserve de débit

L'utilisateur demande de faire le nécessaire pour valider le nouveau montage002. Capteur emprunté001, tous filsDupont repris par breadboard selon lui ;001 déconnecté. Identité SN/UID002 et imageN8E53036B/59e65cd6…bc186/source4307b95c entièrement vérifiés, **aucun flash/reset ni reconstruction de N8E** pour ce comparatif. Anciennes validationsPASS sur code inchangé non rejouées ; nouvelles conditions matérielles justifient cette calibration et ce gate nouveaux.

## Configuration/calibration restaurées

Une seule commande1 depuisWAIT2 : WHO7B, bonCID/CSD/capacité, FAT32montée sans formatage, **calibration18µs/199brackets/6577ancres**, aucun FIFO/stop/SDerror. Recalcul indépendant des199brackets :18µs exact, spans1897559µs/150504ticks identiques. FS±8g conservée, registreCTRL1AC dans les traces. La calibration exige et obtient la configuration FIFO86 : l'ancien refus de configuration est absent dans cet essai. NomsN8E0003/5 absents par f_openREAD, libre suffisant vérifié avant écriture. Aucun résultat de calibration d'amplitude/SI inventé.

## Unique gate2s à4MHz

N8E0003.TR2 créé par FA_CREATE_NEW, préallocation128Kio, session6755700459971025 ; aucun ancien fichier remplacé/formaté/supprimé. Un seul trigger3, débogueur détaché pendant capture ; témoinCOM5 natif passif lié002 (PowerOnline=true), intervalleQPCintérieur1,0451505s, pas preuve de2sSI exactes. Terminal4/run3/error0 à duréeMCUnominale2001484µs,52312natifs/5232sorties31392octets utiles. Zérorail/clip/FULL/OVR/SD/fileoverflow, STOP167µs/drain13844µs,2XYZfinaux sans ancre future nonémis. Fichier finalisé puis **tous les31392octets utiles relus par FatFs**, CRC/index/session/footer etSHAamont conformes, 103sorties numériques préfixe exactes sur1024sources. Pas une extraction par lecteurPC ; intégritéFAT complète/tenueauxcoupures nonqualifiées.

## Réserve conservée : vérificateur global refuse les flags bruts

Le collecteur finit en **AssertionError**, `pass_references=false`, car il exige flags==0 alors que le statut brut agrégé vaut **0x81**. Ce résultat et le vérificateur original sont conservés, non modifiés pour obtenirPASS. La [fiche ST DS12569Rev8 §9.27/table68–69](https://www.st.com/resource/en/datasheet/iis3dwb.pdf) attribue bit7 au watermark et bits1:0 aux bits hauts du niveau ; FULL/OVR/latch sont absents (masque68h nul). Il ne s'agit pas d'un overrun observé, mais le niveau maximal399 est élevé. Workmax983,775µs/1000, service2349,4375/2500, gap2607/5000, SDstep430,65/1000 ; FIFO399/file6144B. Les gardesruntime restent satisfaites, réserve faible pour un long ; pasREADY8h niqualification opérationnelle globale du4MHz annoncée.

RelectureFatFs indépendante du verdicttemps PASS ; `verify_gate.py` reprend le helper existant et corrige **seulement son champ de rapport bytes hardcodé31764** pour relever le compte réel31392. La fonction cible durable_verify_last et ses gardes CRC/SHA sont inchangées. Son retour0 établit l'intégrité du fichier, pas unPASSglobal dequalification.

## Suite techniquement justifiée

Après arrêt/filevide/SDquiescente/SPEoff, comparaison **16WHOà8MHz :16×7B, retoursHAL0** ; prescaler/handle remis4MHz avantdetach. Aucun registre capteur écrit ni calibration/capture pendant ce contrôle. Ce résultat de lecture ne suffit pas à qualifier8MHz ; il justifie de préparer la fréquence de référence8MHz pour retrouver de la marge, préfixe neufN8F, mêmesalgorithmes/FS/v3/limites. Ne pas augmenter les seuils ou la file pour admettre4MHz. Le build ARM ciblé N8F8MHz a ensuite réussi, image53032B/c2dd380d…94c61/source7b815b10 préparée mais non flashée à ce checkpoint ; aucun ancien testPASS relancé. Checkpoint des acquis4MHz publié avant tout flashcandidat8MHz.

ÉtatfinalE : stage4/error0/run3, FIFOoff/writeroff/CShaute/clockfault0, GDBdétaché, imageEconservée4MHz. N8E0003 acquis préservé ; N8E0005 non créé. Aucune opération001/FRAM/COM4/OptionBytes/sécuritéWindows. Succès après recâblage et coupure/remise sous tension : rôle exclusif de la breadboard non démontré, alimentation/état initial et capteurprêté confondus. Les preuves sont locales et versionnées, données zlib sans perte/manifestSHA ; durée8h non lancée.
