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

## Premier résultat et correction de suivi d'horloge

Essai f52a5a8 : 31787 XYZ / 3974 TS, arrêt à 1,206563 s sur écart de conversion
fixe 109 us >100 us. Work335us/service2322us/gap2457us conformes sur ce préfixe,
mais dix secondes non acquises. Maximum apparié2318us : poll1803 (dont
consommation663), RTU513, aucune publication dans ce même callback. N4096
numérique diagnostique, oracle Python : RMS global/X/Y/Z=960/953/102/40mg,
peak997/989/135/79mg. ZéroV/F enRTU, FRAM entière exacte14fb, aucune admission.

Correction ciblée : API optionnelle fifo_event_time_track. La même vérification
100us précède chaque correction. Deux lectures compteur/DWT réellement observées
déterminent la pente, avec compensation de l'erreur de phase mesurée pour
l'intervalle suivant. Pivot continu au dernier timestampFIFO ; aucun événement
déjà publié n'est redaté, pas de grilleODR ni UTC ajoutée. Ce suivi est un modèle
calibré borné, pas une précision SI ou une borne thermique garantie. Tests hôte :
ancien affine fixe en défaut sur modèle +80ppm, suivi10s monotone/causal et
refus au même seuil100us.141tests complets60,50s/ARM etASan/UBSan-O2 ciblés PASS.

Seconde image : instrumentation cycles datation/métrologie distincts, incluse
dans les budgets ; capture des brackets de contrôle, maxima appariés. Qualification
physique à poursuivre ; aucune nouvelle admission R2 avant phase1 entièrementPASS.


## Phase 1 acquise — troisième image

L'essai instrumenté intermédiaire s'arrête sur service2511us, sans atteindre le
premier contrôle d'horloge. Paire fautive2507us : poll1991us dont consommation789,
metrologie556/datation102, RTU513 ; publication0. Source de statistiques et
calculateur encore-O0 alors que caller-O2. Gardes inchangées. Lecture GDB terminale
initialement interrompue par dump d'une plage vide (0contrôle) ; collecte FRAM
complétée àl'arrêt sans rejouer l'essai, hash14fb exact.

Compilation-O2 limitée aux deux feuilles de calcul du profilR4S, mêmes sources
et oracles numériques déjà contrôlés sous-O2/ASan/UBSan. Troisième image physique :
10,001045s,265669XYZ/33209TS,5fenêtres diagnostiques,808RTU,0V/F autoritatifs.
Work333us/service1804us/gap1902us/niveau58, erreursFIFO/STOP0 ; STARTvolatil911us,
STOP232us. Paire maximale1801us : clock30/poll1248 (date64/metrologie201,
consommation351)/RTU513, pas publication.99contrôles horloge, résidumax10us.
Oracle4096 indépendant : RMS959/953/102/39mg, crêtes998/992/138/82mg.
FRAM262144 exacte14fb,0write/0tentative/0commande ; phase1PASS borné seulement.

Phase2 : audit/coupures/anti-rejeu sur snapshot authentique avant tout traitement
R2 physique. Le capteur est arrêté ; poursuite entreprobes isolés fermés avec
backupM016 vérifié, restauration complète àla fin, sans restauration intermédiaire
inutile avant chaque flash de probe. Aucune entréeLIVE générique sur un pont.
