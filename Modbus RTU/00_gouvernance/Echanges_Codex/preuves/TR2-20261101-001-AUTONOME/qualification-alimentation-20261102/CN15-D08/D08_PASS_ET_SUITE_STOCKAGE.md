# D08 — minute capteur/FIFO autonome CN15 seul : PASS

Utilisateur confirme CN15secteurseul, STLINKdébranché, JP6USB7–8, JP2retiré,
VDD/VDDIO3,315V, WAITvertclignotant ; USERsurinstruction, vertfixe aprèsessai.
Firmware D01 identique/sourceacb142794570ded5abd15f6521eb5d8b644590e8,
BIN50912B/SHA6a00ec329c3b16cbf1c01fc3661014ad6f1fed05732a60fecbc338d6a7bb98d2,
FS16/SPI3 8MHz/FIR107Q15dec10/limite100µs et budgets inchangés.
Pas de GDB/Windows/STLINK dans l'exécution selon montage confirmé ; départUSER.

Première demande de reconnexion : seulSN002 détecté, aucun accèsMCU001 possible,
aucunaccès002 effectué. Usercorrige le branchement puis relevé parSN001/UIDexact,
sansresetflashSTARTagent. Snapshot lu enpremier, CRCb4bd1b0f conforme,67488B,
SHA5ec11cf108ce5c426ae3b3a388a916126f936a4627262f4c82849e613704ff27,
sourceacb exacte et différent du snapshotD07 (pas de réutilisation d'ancien succès).
Readback entier duBINstrictementidentique. Les scripts/logs de l'échecUSB initial
sont conservés séparément ; ils ne représentent pas un échec de l'essai physique.

Résultatcapturé : **60000079µsMCU, stage4/error0,1585752natifs/158576sorties**,
598contrôles/résidumax20µs≤100, calibrationinitiale41µs, FIFOerror/stop_error0,
STOP133µs/stoppedtrue, rails/clips/flags/queueoverflow0, clockfault0/budgetmask0.
Maxima polls completsµs : service702,0125/consumer372,56875/gap962/appelwritervide1,01875.
PasdecommandeSD parprofil/gardeinvariant, writer capturéinactif ; zéroarchive créée,
951456B est un volumecalculé, pas écritSD. TousXYZnonpersistés ; digest etsnapshot sauvés.

Au relevé, étatcourant auWAIT2/run1/error0/cal33µs : un nouveau démarrage est donc
observé entre la fin capturée et le relevé, cohérent avec les observations antérieures
de reconnexionSTLINK ; sa cause exclusive n'est pas établie (CSRcumulé1c004400).
Le snapshotNOLOAD reste valide, ne pas substituer les compteurs remisàzéro au résultat
capturé. Aucune coupureCN15 n'est rapportée ; pas de resetagent. FIFOactueloff/stoppedtrue,
writerinactif, SDcommands/reads/writes0/CShaute, GDBdétaché. Aucun002/FRAM/COM4.

PASS physique borné de la minute capteur/FIFO autonomeCN15, pas de qualification
stockage, endurancelongue, tensioncontinue, duréeSI ou powerbank. Aucune répétition
D07/D08 réussie requise. Suite : repriseimage d'enregistrement512Go existante B02,
60s/FS16/SPI3 8MHz/SD4MHz, nouvelarchiveB020005.TR2, puis vérification complètePC ;
préflightCREATE_NEW/CID exact/freespace/guards conservés, pas de formatage.
B02 source3f803d77 déjàbuildée, jamaisflashée dans les preuves jusqu'ici ; vérifier
empreinte/provenance/cible avant réemploi, sans refaire les tests inchangés.
Aucun START de stockage dans D08 et aucune nouvelle image chargée à ce checkpoint.
