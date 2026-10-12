---
mission_id: TR2-20261013-M015-P-COMPARATIVE-FIFO-ROOT-CAUSE
status: BLOCKED
base_ref: main
base_sha: c3e85afbd32f38f07687d2f12cf07a96fc2ed6ee
initial_head: c14d3c60f4c42dc7eba2a1712633c656b18965a8
result_sha: 6546a1bf11ffd35fc2c06079f7da9769867d1973
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-P — FIFO XYZ observé ; cause précise non établie

## Résultat et décision

Le contrôle unique sur la seconde NUCLEO reçoit **263 XYZ et 33 timestamps**, soit 296 mots FIFO. Les 263 triplets sont distincts ; parité, discontinuités des compteurs et composantes saturées : zéro sur cette capture. Le décodage indépendant retrouve exactement les octets extraits des transactions SPI. Entre timestamps : huit XYZ, pas de timestamp de 24 ticks. Il s'agit d'une **qualification physique bornée**, pas d'une preuve globale de perte zéro, de répétabilité, de fréquence mesurée, ni des fenêtres 512/4096 ou de la chaîne V1.

Le BIN de contrôle est **strictement identique** au BIN M015-O niveau10/mode3 qui avait reçu 258 timestamps et zéro XYZ : 54 780 octets, SHA-256 `ed19310485d51af22b18420ea8af3ff3acba36531b3e9741bc78e91792f282f5`. Build, image historique et Flash relue ont été comparés. Le checkpoint logiciel évalué est `b79fe8d5ba495b14ec15379b111ccb4a948faabf` ; le code historique O évalué était `906de1ad85f6b3d7d6737bc699f29a482590614d`.

L'utilisateur rapporte : « J'ai vérifier le branchement de la nappe du capteur, débrancher et rebrancher ». Cette intervention différencie le contexte O/P. **L'association reconnexion/récupération XYZ est observée ; sa causalité précise reste inconnue.** Un contact rétabli et un changement de l'état initial du capteur, éventuellement par alimentation, ne sont pas distingués ; aucune coupure d'alimentation n'a été mesurée. Ni panne du capteur ni défaut exclusif du transport SPI ne sont démontrés.

Le contrôle positif rend l'essai causal duplex non discriminant pour l'échec historique. Son image reste **compilée, jamais flashée ni exécutée**. Arrêt de la campagne, sans autre acquisition ni manipulation : le statut contractuel reste **BLOCKED / diagnostic partiel** parce que la cause racine n'est pas établie, tout en conservant le succès FIFO XYZ réel.

## Audit et modifications

Le [comparatif I/J/O](../../../experiments/iis3dwb_st_reference/COMPARAISON_M015_P.md) documente sources, lignes, commits, registres, transactions et matrice d'hypothèses. Les bruts I reconstruisent 256 XYZ réels ; BIN/ELF historiques ont été rehashés. Les 128 mots XYZ reçus en J furent rejetés avant admission du parser après FIFO_CTRL3=14 contre 0A et CTRL4=0C contre 06 : zéro accepté ne signifie pas zéro reçu. Les tags bruts réfutent le décalage proposé >>2 ; >>3 distingue XYZ et timestamps. Cartes/modules, horloges, FS, séquences et arrêt du capteur diffèrent : ces facteurs ne peuvent être confondus avec une preuve causale.

La seule variante préparée compare commande/payload HAL séparés à un appel duplex continu, sans changer la configuration ST. Constructeur de trame pur, option BENCH_DUPLEX=0/1, traces et gardes de longueur/erreur/OVR/budgets/cleanup conservés. Aucun patch du driver vendor ni du firmware primaire. Le contrôle byte-identique confirme l'absence de changement effectif de son image.

Validation proportionnée déjà acquise : un CTest ciblé réussi, conservant 14 cas existants et huit cas de budget, avec huit nouveaux cas de trame ; deux builds ARM contrôle/duplex réussis. Logs et commandes conservés. Aucun test ni build réussi n'a été répété après l'acquisition ; cœur TR2 et interfaces de production inchangés.

## Opérations et limites

Seconde carte seule : ST-LINK `0032001F3235510C37333439`, UID `001B00463842501820333157`, identités contrôlées ; flash standard 0x08000000/verify/readback qualifié, reset GDB préparatoire et un seul déclenchement contrôle. CPU16MHz/SPI2MHz/mode3 selon configuration ; absence de mesure indépendante des signaux.

Résultat cible : stage6/result0/cleanup0 ; 47 traces, 2 139 octets journalisés, 94 snapshots avant fermeture, aucun overflow ni refus de journal. Configuration 07..0A=00/01/0A/86, CTRL1=AC, CTRL3=44, CTRL10=20. Cleanup relu : CTRL1=0C, FIFO_CTRL4=80, CTRL10=00 (XL arrêté, FIFO bypass, timestamps arrêtés).

Le script de capture GDB termine avec erreur sur `bench_transfer_method`, symbole éliminé par le linker dans le contrôle. Les données nécessaires avaient déjà été sauvegardées. L'erreur est conservée ; la méthode provient de la configuration compilée, pas d'une variable lue. Analyse hors ligne et session finale **en lecture seule**, sans reset/flash/trigger, confirment résultat inchangé, UID, faults0 et detach réussi. La seconde carte reste sur l'image de contrôle inactive ; aucun backup/restauration 2 MiB, mass erase ou Option Bytes.

TR2 principal, M014-H, FRAM, microSD, partition et Modbus/HP jamais accédés. Archives mission/rapport O conservées exactement ; worktrees historiques et deux stashes préservés. Aucun essai antérieur réussi n'a été rejoué.

## Preuves et prochaines actions

[Corpus versionné](preuves/TR2-20261013-M015-P/README.md) : bruts, CSV, journaux flash/GDB, comparaison d'images, analyse indépendante, builds/tests et intervention rapportée. Originaux locaux : `/home/lolo/dev/msm/reports/TR2-mission015-P-20261012T221333Z`. Les échecs N/O restent des observations historiques distinctes, sans réécriture.

Pour une mission suivante : conserver le montage actuellement fonctionnel ; qualifier une répétabilité bornée puis les tailles de fenêtre avec les réserves de cleanup. Si la cause historique précise est indispensable, séparer état d'alimentation/reset du capteur et contact de nappe par observations indépendantes au module. Ne pas provoquer un mauvais contact pour reproduire l'échec. Aucune nouvelle campagne n'est engagée ici.

Journal et roadmap actualisés : acquis secondaires 263 XYZ, contexte utilisateur, cause encore ouverte, arrêt du causal. M015 reste IN PROGRESS ; qualification continue, FFT, stockage et publication vibratoire V1 restent à démontrer.

## Publication

Checkpoint logiciel `b79fe8d5ba495b14ec15379b111ccb4a948faabf` publié sur main et 26 fichiers relus intégralement, byte-identiques, avant matériel. Résultats publiés par push normal au commit `6546a1bf11ffd35fc2c06079f7da9769867d1973` (« M015-P: preserve positive FIFO control and bounded root-cause findings ») ; les 59 fichiers modifiés ont été intégralement relus depuis origin/main et sont byte-identiques. Voir publication-readback.json. Worktree de mission propre après ce commit ; dépôt principal et stashes préservés. Le présent complément documentaire consigne cette vérification ; son SHA final est communiqué après sa propre relecture distante. Permissions commit/push/flash/debug exercées uniquement dans le périmètre du contrat.
