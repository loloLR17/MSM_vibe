# Après remplacement des fils du capteur — fréquences SPI discriminées

Utilisateur confirme : tous les fils du module capteur remplacés, ceux du lecteurSD Adafruit4682 **inchangés** ; même brochage, VDD/VDDIO3,3V au multimètre, carteSD enplace, STLINKbranché, capteur toujours emprunté àTR2-001. Le nom de module saisi par utilisateur est «IIS3DWB/MKIS08V1» ; pas de nouvelle identification unique de carte inventée. Brochage et tensions sont des observations utilisateur, pas des mesures automatiques.

**Aucun build/flash/reset logiciel dans cette tranche.** ImageN8A54556B/c31904fe…3ddc17 relue entièrement identique ; UID002 etWAIT2/run0/error0, CSR0x0c004400 avant toute commande. État correspondant à un reboot constaté après intervention ; flags cumulatifs ne permettent pas de réattribuer le premier reset de la veille.

Problème hôte séparé : serveurGDB ne peut pas lier61236/61237, plageWindowsnetshTCP61235–61334 exclue, connexionGDB nonétablie. Deuxports23336/23337 libres testés par bind/release, puis seulepaireTCP duhelper changée. MêmeSN/SWD8000/attach/outils ; aucune exclusion/firewall/protection modifiée, aucune miseàjourSTLINK. Le contrôle cible a ensuite fonctionné.

Initialisationcommande1 arrêtée **avant calibration et SDinit** : WHO0x7A au lieu0x7B, stage99/error91/run0. ZéroFIFO/SD/écriture ; aucunecalibration complète ni collecte. Garde d'identité nonassouplie.

Lecture seule WHO, sans écriture de registre capteur, flash, reset ou nouveauSTART :

| Série | FréquenceSPI nominale | Résultat |
|---|---|---|
| Initialisation |8MHz|0x7A, refus|
|16lectures|8MHz|16×0x7A|
|16lectures après seul prescaler|1MHz|16×0x7B|
|16lectures après retourprescaler|8MHz|16×0x7A|
|16lectures après seul prescaler|4MHz|16×0x7B|
|16lectures après retourprescaler|8MHz|13×0x7B et3×0x7A|

TousretoursHAL0, sans garantie de validité électrique desoctets. r3_result.who original0x7A/error91 préservés ; les lectures favorables ne sontpas réinjectées danslesétatsmétier. SPI3prescaler ethandle remis à8MHz avantdetach. Cescourtes séries montrent une dépendancefréquence/reproductibilité des erreurs ; elles ne qualifientpas une durée8h à4MHz et n'isolentpas forme d'onde/charge/driver. Le nombre de lectures etlesrésultats sont exacts, pas une probabilité d'erreur universelle.

La cadence26,7kXYZ/s/±8g/FIR/decimation10 ne change pas lors de ceslectures. À1MHz le débit n'estpas une solution d'acquisition admise. Prochaine trancheautorisée : profil4MHz avecvoie dupleximplémentée, buildexact/calibration puisgateFIFO+SD avecmêmeslimites ; aucunecapturelongue avantqualificationdugate etsignalLANCE8H. Lesanciens4MHzsplit ontéchoué encharge, résultat préservé, nepas extrapoler deWHO correct.

Bruts sansperte en zlib etmanifestSHA ; contrôlesgraphiques sansobjet. MCU reste99/error91/sanscommandeSTART ; writerinactif/SDCShaute, GDBdétaché aprèsretour8MHz. État ODR du capteur nonprétendu qualifié par unWHO erroné ; aucune commandeODRON envoyée danscetteinitialisation. Anciens fichiers/données etlogicielTR2-001/FRAM/COM4 inchangés.
