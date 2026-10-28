---
mission_id: TR2-20261028-ACCEL-FS8G-VIBRATION-AC
status: READY
base_ref: main
base_sha: e7f17f2cd2b8470244a99f7c9e73e9d0f43b3dea
created_at_utc: 2026-10-28T22:10:00Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# MSM TR2 — Élargir la plage d'accélération à ±8 g et préparer une vraie métrique vibratoire

## Décision utilisateur impérative et motivation

L'utilisateur confirme le 28 octobre : le capteur n'était **pas soumis aux vibrations de la machine cible** pendant les qualifications précédentes. L'absence de saturation à ±2 g dans ces essais n'est donc **pas une justification pour conserver la plage ±2 g**. Il exige une plage couvrant **au moins ±6 g**. Comme l'IIS3DWB propose ±2/±4/±8/±16 g, la cible concrète est **±8 g minimum en utilisation nominale**, sans modification automatique à ±16 g sans motif. C'est une **exigence de conception validée par l'utilisateur**, non une hypothèse à revalider. Ne pas refuser le changement au seul prétexte que l'essai au repos ne saturait pas.

Deuxième point à traiter sans confusion : la calibration actuelle `s_calibrate` est une synchronisation temporelle FIFO et **ne retire pas la pesanteur**. Les RMS/crêtes B3 actuels sont absolus et incluent le DC ; fenêtre physique 4 096 samples ≈954 mg RMS absolu, après centrage XYZ *hors ligne seulement* ≈19,391 mg. Concevoir une mesure exploitable des vibrations (composante dynamique) **sans changer subrepticement les définitions normatives gelées ni transformer les historiques**. Le P2 START/STOP Web reste documenté et indépendant, hors périmètre prioritaire de cette mission.

## Référence et préservation A-06

Lire `AGENTS.md`, `GOUVERNANCE_CHATGPT_CODEX.md`, protocole des échanges, état courant/roadmap, spécifications B3/B4 gelées, la mission précédente archivée `TR2-20261019-DEMO-METROLOGY-UX`, son dernier rapport, `Supervision TR2/LIVE_METROLOGY_UX.md`, preuves brutes et tests métrologie. Vérifier HEAD réel main. Firmware LIVE physiquement installé à la dernière vérification = source `c36c9718a75e6b40c45707a482e8db836296cab2`, BIN SHA `9bbe397911185b798a0b030aba893a325bfdf6afa6947145bda9157f3bff0981`. Configuration active rapportée `config3/gen3`, full_scale_code 0 (±2 g), FS capteur éteint relu après STOP; 26667 Hz, 4096 échantillons, period 2000 ms. Dernier rapport daté 28/10 : interruption d'alimentation accidentelle confirmée par l'utilisateur, retour STOP/séquence 0 ; les relevés matériels sont datés, **ne jamais les présumer actuels**. Service Windows d677/Service.dll SHA63f3dd…fa sur localhost5100, UI statique rénovée, Smart App Control actif, comportement des nouvelles DLL non garanti. Préserver ledger transactionnel (dernier STOP 5/25 historiquement), FRAM et ses identités, Flash de récupération, archives, données SQLite, lanceurs et version démontrée.

## Lot A — élargissement effectif de plage sans perdre la qualité

1. Auditer la chaîne *réellement exécutée* : configuration persistante/validation B4 et sources de vérité, initialisation FS de l'IIS3DWB (chemin FIFO, pas seulement vieux driver polling), contrôle registre `CTRL1_XL` **pendant acquisition si lecture qualifiée sans interruption** ou lors de configuration autorisée, conversion `raw→mg` / overflow et saturation jusqu'à ±8 g, RMS/crêtes 32 bits B3, seuils et tests. Documenter la sensibilité `±2g=61 µg/LSB`, `±8g=244 µg/LSB`, compromis résolution/bruit/saturation.
2. **Mettre en place une configuration nominale ±8 g réellement active**, par le chemin transactionnel ou configuration qualifiée déjà prévu, sans écriture FRAM brute ni modification silencieuse du mapping protocolaire. La mission autorise la mise à jour de configuration nécessitée par la décision utilisateur, avec maintien de l'epoch/identité, journal/CRC/autorités et historique. Si l'actuel firmware/profil opérateur ne permet aucune mutation de B4 sûre, réaliser la modification firmware et la migration **minimales**, prouvées/reproductibles, en protégeant l'état persistant ; ne pas improviser un patch RAM non durable. Si une opération destructive inhabituelle ou un risque matériel non maîtrisé apparaît, bloquer explicitement et demander uniquement la décision importante nécessaire.
3. Tests unitaires/host et ARM ciblés, oracles indépendants sur conversion ±8 g / saturation positive et négative, unités mg, bornes V1 et seuils, constantes et registres FS, no overflow RMS/crête, arrêt/redémarrage et reset/power-cycle contrôlé si sûr. Comparer ±2/±8 sur sollicitation raisonnable et **réellement disponible**, sans prétendre avoir reproduit les vibrations de la machine cible ni inventer un essai ±6 g. Valider le **mode choisi même sans excitation forte** par la configuration et la lecture retour du registre, la conversion/oracle et le chemin B3→SQLite→UI. La couverture **effective ±6 g en conditions machine** restera à confirmer lors des campagnes cibles.
4. Mettre à jour l'IHM/diagnostic pour afficher la plage **configurée et effectivement prouvée** si accessible, et indiquer que les indicateurs sont toujours RMS/crête absolus tant que le retrait DC n'est pas qualifié ; ne pas afficher «±8 g effectifs» uniquement sur base d'un paramètre si le registre physique le contredit.
5. Préflight non destructif du STM32/COM4 et état acquisition/ledger ; STOP terminal avant toute intervention avec preuve, GDB sûr hors scope selon `operator-demo/COM4_DIAGNOSTIC.md`, aucune lecture FRAM longue injectée dans un callback chronométré, contrôler B0/B3 **après** audit sans reset. `allow_flash:true`, `allow_debug:true` avec garde-fous AGENTS : Flash/FRAM/SD/OB sauvegardés et préservés, aucun rollback/journal retiré, aucun RENEW/anciennes clés B5, conserver image stable récupérable. Valider le nouveau firmware physiquement **uniquement** avec commande STOP disponible et transport COM4 qualifié.

## Lot B — métrique de vibrations débarrassée du DC/gravity, sans ambiguïté

1. Définir avec preuves le besoin et la sémantique : composante constante/gravité/biais capteur, soustraction **moyenne par axe dans fenêtre** versus calibration au boot, fenêtre et filtrage passe-haut, bruit, fréquence minimale exploitable et risque de retirer des accélérations réelles lentes. **Ne pas prétendre qu'un centrage court supprime « la pesanteur » dans toutes les conditions**, ni supprimer 1000 mg du RMS global ou déduire une calibration physique du seul normeuclidien.
2. Produire proposition technique vérifiable d'une **métrique vibratoire dynamique distincte** de RMS/crête B3 absolus, avec unités mg, indicateur RMS dynamique XYZ global et axes, crête dynamique cohérente, règle de validité, orientation, durée et estimation du DC, métadonnées/provenance et tests sur fixtures déterministes. Le protocole B3 et contrats gelés **ne changent pas** sans validation structurante explicite ; si un affichage dérivé hors ligne est possible de façon mathématiquement honnête et fiable, livrer comme prototype **clairement étiqueté expérimental**, sans masquer les valeurs officielles ni prétendre que les snapshots B3 suffisent pour reconstruire le DC d'une fenêtre. L'implémentation canonique complète pourra constituer une mission normative distincte si nécessaire.
3. Fournir au responsable projet une proposition de choix métrologique (fenêtre, fréquence de coupure, définition RMS/peak, recalage au START/boot, validation avec vibrations cibles), sans demander de validation répétitive pour les détails purement logiciels. Les décisions de changement de grandeur publiée dans blocs gelés restent escaladées.

## Déploiement et non-régression

- Toute nouvelle publication PC Windows doit être **complète** et passée par gate vérifiant 4 HTTP 200, SAC actif, zéro blocage Code Integrity corrélé, DLL/EXE SHA, sans mélanger les fichiers de l'application actuelle. Conserver l'ancien paquet accepté et une voie de retour. Si la mise à jour concerne seulement un affichage et qu'elle est prouvée, privilégier assets statiques. Ne jamais désactiver/contourner Smart App Control ni affirmer que les futurs builds sont automatiquement approuvés.
- Conserver en priorité les écrans LIVE récemment rénovés, leurs tests JS/Edge 1080p, la communication COM4, CSV, l'âge des HELD et la distinction état connecté/fraîcheur. Les commandes opérateur START/STOP fiables restent disponibles ; ne pas ajouter de faux boutons ou de deuxième maître. Aucune nouvelle commande quand état cible incertain. Nettoyage des ~73 worktrees/stashes/main divergent hors mission.
- Publication normale sur main, preuves physiques et logicielles datées, nouvelle configuration prouvée par lecture retour, comparaison avant/après des indicateurs et résolution/FS, état final STOP avec firmware retenu si sûr, et état Windows. Si impossible de garantir les autorités persistantes, publier BLOCKED honnête avec solution détaillée, sans annoncer ±8 g opérationnels par simple modification de source.

## Critères d'acceptation

- **A** Plage **±8 g nominale effectivement sélectionnée et vérifiée** par configuration, registre matériel réel et conversions correctes ; compatibilité B3/FRAM/identités, START/STOP et HMI sans régression. Distinguer plage de mesure/« calibration » et préciser qu'une sollicitation réelle ±6 g n'a pas été reproduite si c'est le cas.
- **B** Statut net de compensation gravité : **non présente actuellement** ; conception technique et, si possible sans changer contrat normatif, prototype de métrique AC clairement séparé. Ne jamais annoncer un capteur compensé ou des vibrations RMS corrigées sans preuve.
- **C** Respect protections matérielles, Windows SAC, états historiques, intégrité des preuves et service LIVE ; remontée des obstacles vraiment structurants.
- Rapport `DERNIER_RAPPORT.md` avec DONE/BLOCKED précis, liens vers artefacts/tests, commits, empreintes, audit état matériel final, relecture distante. ChatGPT contrôle indépendamment les résultats avant qualification.
