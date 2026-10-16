# R4-S — diagnostic de service puis reprise conditionnelle

Mission TR2-20261016-R4S-SERVICE-BUDGET-SAFE-RECOVERY, départ20c8089.
Phase1 seulement dans cette tranche : probe dédié NON-PRODUCTION, garde SHA256
FRAM14fb172f2917d0a8e440438051831c84f3f3e746161702a7de9e9a1a251b2aa9,
port média avec écriture refusée et compteur de tentatives. Recovery réel R1/R2
relu ; STARTED epoch4/tx5/run5 ne change pas. Aucun submit/step de commande.

Réemploi du chemin R4-R/83d4fbd : provider, métrologie, transport SPI et RTU RAM ;
entrée R4-R paramétrée pour partager ses helpers, sans appeler son ancien run.
Calibration2s, acquisition diagnostique10s avec quanta FRAMreadonly32octets.
Identité volatile UINT64_MAX-1 sans réservation R2 ; autorité de la métrologie
forcée false avant toute publication : B3 ne doit jamais annoncer V/F. Les
nombres diagnostiques et4096XYZ bruts sont conservés pour un oracle indépendant,
sans prétendre une mesure LIVE autoritative dans ce contexte indéterminé.

Mesures appariées par service : horloge, pollFIFO incluant consommation,
consommation/datation/calcul, publication et8pollsRTU, total. Maximum total
apparié conservé séparément ; work1000us/service2500us/gap5000us inchangés,
START volatil contrôlé<=1000us ; STOP/readback sur fin ou faute. GDB attend le
breakpoint terminal après STOP, aucune interruption pendant le transfert.

Validation logicielle : profil/refus CMake, ARM dédié/canonique, provider et
métrologie O2/ASan/UBSan/oracles avant cible. Ancienne suite141tests préservée,
non répétée pour ajout strictement harness. Les phases2/3 restent conditionnelles
au succès physique du service et à la preuve d'applicabilité de la maintenance.
Restauration Flash M016 complète et FRAM inchangée à vérifier après ce probe.
