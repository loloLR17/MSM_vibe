# TR2-002 — acquisition 2 h OBE lancée, checkpoint initial

## Autorisation et séparation des cibles

Utilisateur demande « arme TR2-002 pour2h […] modeST-LINK comme[…]jusque-là », puis
« lance aussitot que tu es pret ». Extension humaine explicite de la cible002 pour
programmation/qualification préalable minimale/START2h, sans intervention001. Le
contratChatGPT n'est pas réécrit. TR2-001 C29 surCN15 reste une capture supposée,
départ12:12UTC rapportéuser/LEDéteintes/nonvérifiée ; aucune session ou commande001
réalisée dans cette tranche. Ne pasconfondre son état avec l'ACTIVE002 mesuré.

## Image et préparation002

SN0032001F3235510C37333439/UID001B0046/38425018/20333157 exacts, ancienN8GauWAIT,
FIFO/writerinactifs/nonincertains. LectureFlashcomplète2097152B préservée localement,
SHA113f54b740a2ee2be789ac42649becb8fe58b8a47d33890c587faabf405386e5, identique à
la sauvegarde002 antérieure. Profil002historique ±8g/244µgLSB/FIR107Q15dec10/SPI3 8MHz/
SD4MHz, gardes inchangées ; nouveaupréfixeOBE. BuildARMcibléPASS, source dd2f27ca3d886742b0ab37ab378cd524012a0370,
BIN/empreinte dansartifact.json. Unflashstandard-v-rst vérifié, pasd'effacementmassif/OB.

Init/calibration puis gardesnomsOBE0003.TR2/OBE0005.TR2(absenceFR_NO_FILE4 viaf_openFA_READ)
et libref_getfree906719clusters≥20484. Carte32Go CID/CSD/capacité exacts62333952secteurs,
FAT32cluster64secteurs. Aucunformatage/suppression ni affaiblissement descontrôles.
Le premier script GDB a appelé durable_preflight, absent du BIN002 lié ; correction
par la procédure lectureseule f_open/f_getfree historique, sans refaire l'init réussie.
Une erreur de génération locale du scriptcorrectif a été corrigée avantexécution ;
aucune commandeMCU supplémentaire associée à cette erreurPython. Délaioutillage,
pas un défautmatériel observé. Les tracesréelles sont conservées.

Gateobligatoire2s cmd3 exécuté unefois, puis durable_verify_last(r3_source_sha)=0 :
fichiergate SDrelu et digestproducteurRAM comparé par lefirmware. Ce gate est le
prérequis duSTART5 historique, pas une réexécution desanciennescampagnes. Résultats
binaires/header/footer/digest conservés. Pas de prétendue qualificationPC de ce gate.

## Départ2h et témoin

OrdreRAMSTART5 unique soumis, GDBimmédiatementdétaché. **ACTIVE + premiergroupeSDacquitté
confirmés le02/11/2026 à12:30:12.839818UTC** (annotation de confirmation, pas UTCdeséchantillons).
PremièretrameQPC2 valide UID002/run5/flags1281/session8444550119014285/accepted12808.
Fichierlong **OBE0005.TR2**, garde7200000000µsMCU/2hnominales/préallocation640MiB.
Le débit etla duréeSI ne sontpas déduits de cette seuleannotationUTC.

TémoinWindowsnatif PID15364, COM5 parentSN002 vérifié, PCsursecteur et veille/
hibernationACdésactivées constatées sansmodification. InterrogelectureQPC2 sanscommande
START/STOP/RESET, continueindépendammentdeCodex. MinimumintervalleintérieurQPC7100s :
preuvepartielle distincte, **pas uneassertion7200sSI**. Ce paramètre n'altère aucunegarde
MCU ni duréeprogrammée. Après lecheckpoint, aucune sessionGDB/WSLmonitor pendantcapture.

Journauxnatifs : C:\Users\lolo\AppData\Local\Temp\TR2-R3-N8G-20261101-OBE-2h
Local : /home/lolo/dev/msm/reports/TR2-002-2H-OBE-20261102
Àconserver : STLINK002 connecté/T1000sursecteur éveillé, pasveille/redémarrage/débranchement.
Ne pas couper après2h sans contrôlerterminal/sauverdigestRAM :002n'a pas le snapshot
NOLOAD autonome001. Récupération àla fin par résuméUART puisGDB aprèsSTOP, SDquiet,
dumpcompteurs/digest, détachement, ensuite seulement transfertSDphysiqueuser.

Repèreopérateur : **« passage sur mode constante » à12:33UTC**, signaléutilisateur,
précisionminute/scopephysique nonprécisé. Aucun changement logiciel associé.

## Limites du checkpoint

CaptureACTIVE initiale mesurée, **2h nonterminées/nonqualifiées**. Pasdelecture du
fichierlongPC ni de résultatfinal inventé. Aucune001/FRAM/COM4/Windowssecurity touchée,
aucune ancienne donnée effacée. Les artefactsvolumineux restent localementT1000.
