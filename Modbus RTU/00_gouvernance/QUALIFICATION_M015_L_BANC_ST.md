# M015-L — chaîne seconde NUCLEO et résultat WHO

## Identification et procédure effectivement exercée

Seconde NUCLEO-U575ZI-Q uniquement : ST-LINK0032001F3235510C37333439/V3J15M7,
UID001B00463842501820333157, STM32U575-U585 ID482 RevU/3,28V via CLI. Les identités
primaires SN003500463235510B37333439/UID004A003D3842501820333157 sont refusées.
Confirmation utilisateur : «3,3V sur les deux VDD, ground sur la broche GND, et
cablage correct MKIGIBV2 conformément à ta demande». PointGND marqué, numéro
non communiqué ; câblage physique annoncé conforme, pas inspection électrique
indépendante par Codex. La réponse précédente «tout bon normalement» n'avait
pas levé le préflight ; aucune connexionMCU avant la confirmation ciblée.

Chaîne exercée : CubeProgrammerWindows2.23/GDBserver7.14/GDBGNU CubeCLT1.22,
NORMAL/SWD/sn explicite/freq8000, programmation08000000/verify/reset ; GDB
avec `-d -g -i <SN_seconde>`, port61234, reset aprèsdernierCLI et ensuite GDB
seul. Source e222dc8, WHO BIN31632 SHA375fd90243d34eedb9948aff2ba43daabf24c0d0f0a326be2bbee49c1d8f9c21,
ELF/hashmanifestK vérifiés. Flash2MiB fraîche doublelecture exacte sauvegardée
avantCLI/flash, SHA fcea004eb0c91af44f8e36ca61206b5f6bf25fd4c5db4dfdab42d1a19dcea9d9.
RAM initiale768KiB conservée. Cette traçabilité documente la chaîne opératoire
sur ce SN/cible/image ; elle n'étend aucune autorisation à une mission future.
Le driver/capteur ne sont pas fonctionnellement qualifiés par le flash.

## Observation unique et arrêt

FlashWHO0..3 secteurs, verify/readbackbyte-identique ; bootready1/UIDexact/
core16MHz/faults0. Unique trigger4D31354B, detach pendant exécution. Résultat
stage6/BENCH_WHO2/cleanup0, WHO reçu00/attendu7B, traceunique reg0F,len1,write0,
HAL_OK/error0/counts0/valid1 ; RXpayload00 conservé. Zéro write capteur, zéroXYZ.
CSODR/IDR PC9 haut en fin, MISO PC11digitalbas à ce snapshot ; pas mesurewire,
pas preuve de toute transition. Commande8F reconstruite du code, RXcommandeabsent.
SPI3/mode0/16MHzdiv8=2MHz calculés, pas mesurefréquence.

STOP au premier écart : aucune image polling/FIFO/STATUS10 flashée ou essayée.
Les gettersST connus ne sont pas atteints ; cause du WHO00 non établie.

CaptureinitialeGDB interrompue sur macroGPIOC non exportée enDWARF ; correction
strictementlecture par adresseNS42020800 vérifiée CMSIS, complément sans nouveau
trigger/reset/flash ou lectureSPI. Payload et traces intacts avant restauration.

## Restauration et limites

FlashaprèsWHO entière lue : hors08000000..08008000 byte-identique au backup.
Seuls secteurs0..3 restaurés depuis backupfrais32KiB ; verify réussi puis
readbackFlash2MiB intégral byte-identique/SHAfcea004e… . ResetGDB aprèsdernierCLI,
PC08000c80/VTOR08000000/UIDexact/CFSRHFSR0 observés, detach etserveursexit0.
Firmwareinitial non identifié fonctionnellement : restaurationdesbytes/étatdebug,
pas validationapplicative. Aucun accès auTR2primaire, média, RS485/COM7/HOTPLUG/
masserase/OptionBytes/RDP ; M014-H demeure inchangé sansrequalification.

Statuttechnique BLOCKED. Préflightlogique renseigné puis échec réelWHO,
pas un abandon pour seuleincertitude avantflash. Diagnosticpréparatoire suivant :
continuités hors tension MISO JP2-24→PC11/CN8-8 et orientationnappe pin1 auxdeux
extrémités, VDD/VDDIO etGND jusqu'àcarteMKI208 à partirdu schéma exact, labels
réels et éventuelles résistances/transitionsCS/SCK siinstrumentidentifié. Ne pas
attribuer panne module/SPI/HAL ni rejouerWHO sans décision de reprise spécifique.
I256 validé/Jconfigurationpostburstrefusée portent surautrebanc/paramètres ;
la voieisolée n'a pas franchiWHO, aucune comparaisoncausale acquise.

Preuves Echanges_Codex/preuves/TR2-20261012-M015-L ; originaux
/home/lolo/dev/msm/reports/TR2-mission015-L-20261012T195811Z. CodeK etvalidations
41C/5Python/4ARM inchangés etréutilisés sansrejeu.
