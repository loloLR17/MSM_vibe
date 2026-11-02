# B02 — enregistrement 60s autonome CN15, préparé

Après D08PASS, reprise de l'image déjàcompilée B02, source3f803d77393c6853ed76d796367df443a0eb9eb9,
BIN53312B/SHA59a236ae0d8f3d8f6b492fc84914cbf33cc5eaa304aaf13eebdb4e222f41cffb.
Durée60s/USERunique, FS16/488µgLSB, SPI3 8MHz/SPI2 4MHz, garde100µs et budgets
stricts, CID/capacité512Go/partition/freespace/CREATE_NEW conservés, pas de formatage.
Build/testspreexistants réutilisés sans recompilation/répétition ni changement de code.
Aucun prétendu PASS de la chaîne stockage à ce stade.

Identité001 vérifiée avantflash, D08snapshot sauvegardé/pushé ; unflashstandard
normal + readback53312B entier exact. Lecteur001 vide,512Go auPC pendantflash.
Postboot99/error2/sd_error2/WHO7B, absenceSDattendue : writerinactif/uncertainfalse/
needs_power_cyclefalse/SDwrites0/FIFOoff/CShaute/GDBdétaché. AucunSTART/FRAM/COM4/002.

ContrôlePClectureseule : disqueUSB511868665856B, D:FAT32/label512GO,
partitionoffset1048576/longueur511867584512. AnciensA1A..H/B010005 présents,
B020005.TR2 absent ; pas de nouvelleempreinte de tous les anciens fichiers ni
qualificationCID par cette seule lecturePC. Le firmware vérifiera son CID exact
avant d'autoriser toute écriture. Pas de suppression/formatage/réparation.

Action physique demandée à l'utilisateur : éjecterD:, couperCN15+STLINK001/10s,
insérer512Go sur001, JP6USB/JP2retiré inchangés, CN15secteur5V seul, STLINKoff,
attendreWAITvert sansUSER. Contrôlelibre peut prendre plusieursminutes. Cette
transition n'est pas encore confirmée dans cecheckpoint. AucunnewSTART tant que
WAIT et étatcorrect rapportés. Essai60s puisCN15maintenu/diagnosticRAM/récupérationPC
et vérificationintégraleavant10min. Les mesuresD08 sansSD restent acquises.
