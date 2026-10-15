---
mission_id: TR2-20261015-R0-NORME-LIVE-SUCCESSEUR
status: DONE
base_ref: main
base_sha: null
initial_head: 2b9c0202f79bc132b166776c4aed2a48925593dc
result_sha: 2b9c0202f79bc132b166776c4aed2a48925593dc
created_at_utc: 2026-10-15T09:43:52.214068+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# R0 — Profil LIVE continu spécifié, publié et relu

La finalisation normative est réalisée : **protocole 2, structures B4/B6 version 2, commandes 13/14 et durée LIVE 0 pour fonctionnement continu**. Le profil reprend B5 V1.1 avec epoch, sans réattribuer le code 12. Les campagnes 3/4 et tous les gels historiques restent intacts. Aucun firmware n'est implémenté ou qualifié par cette mission.

Livraison documentaire DONE après publication et relecture intégrale depuis la référence récupérée de GitHub. Le contrôle indépendant de ChatGPT reste nécessaire avant R1, même après livraison DONE de R0. La mission publiée ne contient pas `base_sha` ni `created_at_utc` : champs de contrat manquants signalés, sans SHA inventé ; HEAD effectif et permissions explicites ont été vérifiés.

## Livrables

- [Norme et index](../../01_Specification_source/LIVE_R0/README.md), [profil complet](../../01_Specification_source/LIVE_R0/PROFIL_LIVE.md) : réservations, durée continue, configuration, états/refus, qualité, alarmes, temps, rollover et PC.
- [Migration FRAM](../../01_Specification_source/LIVE_R0/MIGRATION_FRAM.md) : layout existant, nouvelle partition disjointe, identité/configuration/journal/epoch versionnés, coupures et rollback.
- [Mapping et validation](../../02_Validation/LIVE_R0/README.md) : 187 champs / 390 registres, 106 scénarios successeurs et 384 contextes START/STOP, tous fonctionnellement SPECIFIED_NOT_EXECUTED.
- [Contrôles effectifs](preuves/TR2-20261015-R0/document-checks.json), [géométrie et modèle de coupures](preuves/TR2-20261015-R0/layout-review.json), [réservations examinées](preuves/TR2-20261015-R0/namespace-review.json), [réutilisation du corpus A06](preuves/TR2-20261015-R0/corpus-reuse.json).
- Mission et rapport A06 archivés intacts depuis `4eecf373407e939f61307dc44ac538b22bc7fdfb`, sous `archives/TR2-20261015-A06-DECOUPLAGE-SUPERVISION-STOCKAGE-AUDIT/`. Étude et preuves A06 inchangées.

## Résultats et choix normatifs

| Tranche | Résultat |
|---|---|
| Git et sources | main distant 2b9c020, dépôt historique et stashes préservés, worktree isolé. Gouvernance inchangée depuis A06, journal intégral relu, contrat R0/roadmap et dépendances revus. Empreintes des 2 167 fichiers du corpus A06 identiques ; aucune répétition de ses campagnes ou tests. |
| Versions et encodages | Protocole 2/B4=2/B6=2/B7=1 ; B0 bit4 LIVE, B4 capbits0/1 ;13 START_LIVE / 14 STOP_LIVE,12 epoch conservé ; B3 bit8 campagne durable reste 0 en LIVE ; B6 santé4 indisponible. Aucun déplacement/type/accès modifié ; B5 longueur 29 héritée de V1.1. |
| Supervision continue | Tuple(acquisition2, stockage0, limite0, durée0), durée non nulle refusée en LIVE ; aucun minuteur campagne. STOP explicite, arrêt sûr sur défaut bloquant, aucun auto-START après boot, retour SD ou retry historique. Compteurs saturés sans arrêter les calculs ; UTC qui déborde devient invalide sans wrap. |
| États et mesures | Table B1/B3/B6/B7, fautes actives/mémorisées, absence prévue distincte de panne ; qualité indépendante du média. Seuils/hold/hystérésis/arrondis et latch/ACK définis pour LIVE, sans rétroattribuer ces oracles aux zones NOT_DEFINED de V1. |
| Commandes | Priorités contexte définies après contrôles V1.1 ; 384 combinaisons 13/14 développées en oracles. Retry admis avant revalidation métier ; STOP inactif avec nouvelle identité→21, retry exact→résultat historique. Succès START n'est pas redémarrage après reboot. |
| FRAM | Partition historique [0,122880) conservée readonly par LIVE ; nouvelle partition [122880,245760), deux banques et deux superblocks. Image 52298 + header64 tient dans 57344. Propriété réelle du tail à établir avant écriture R1. |
| Migration et rollback | Nouveaux formats configuration2 / identité2 / logique média3 ; journal4 borné sans éviction de l'epoch courante, réserves STOP/renew. Provenance lie historique et LIVE ; ancien firmware cumulatif V2 retrouve son espace intact. Changement historique après rollback interdit l'activation automatique de LIVE ; réconciliation explicite. |
| Compatibilité PC | Ancien PC refuse 2 avant écriture ; nouveau PC choisit explicitement V1 ou LIVE+B5 epoch. SQLite garde observations qualifiées, métadonnées version/config/mode et trous ; pas de fausse campagne ou de promesse exactly-once avec séquence saturée. |

La règle de durée bornée proposée dans A06 est explicitement remplacée par la décision R0 de supervision continue ; le document A06 historique n'est pas réécrit. Le code courant ne fournit pas déjà le protocole 2, la nouvelle identité, la nouvelle partition ni B5 epoch. Ces dépendances sont affectées à R1/R2 et les budgets à R4.

## CRC et contrôles documentaires

| Vecteur | CRC préparé 168 octets | CRC actif 152 octets |
|---|---|---|
| V1_REFERENCE |5207CCFC|092C7610|
| LIVE_A |7BA855C8|7818AAB3|
| LIVE_B |FC4001B4|E5F05014|

Le CRC préparé V1 est la constante historique gelée ; son CRC actif est un résultat documentaire calculé ici, pas une nouvelle constante historique prétendument qualifiée. Tous les registres et SHA sont dans `01_Specification_source/LIVE_R0/vectors.json`. Deux calculs indépendants (bitwise et zlib) concordent, avec relecture du fixture normatif B4. Algorithme, périmètres 168/152 octets, endian et réservés sont conservés ; la version 4000 reste hors CRC et doit être contrôlée séparément.

Commande réellement exécutée depuis le worktree :

```text
python3 'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261015-R0/check_documents.py'
```

Résultat `DOCUMENTARY_CHECKS_PASSED` : 187 champs / 390 registres sans trou ni recouvrement, adresses/types/accès hérités identiques, 17 identifiants d'exigence couverts, CRC concordants, géométrie disjointe et 2 940 fichiers historiques protégés identiques. Le modèle de coupures vérifie chaque préfixe des écritures prévues dans la seule partition LIVE ; **il n'exécute pas les stores ni les barrières du firmware et ne prouve pas l'atomicité physique**.

La revue a corrigé avant publication : une longueur B6 mal recopiée (64 registres réellement), une ambiguïté de sélection entre racine courante et ancien superblock pointant une banque inactive réécrite, et la branche STOP quand son budget journal manque. Le premier contrôle de couverture a signalé deux cas de domaine LIVE manquants (masque et ordre des seuils), ajoutés ; le parseur identifiait aussi à tort le segment R0-NORME du mission_id comme exigence, filtre corrigé sur les identifiants normatifs déclarés. Aucun oracle ni test ancien affaibli.

Validation proportionnée : contrôles documentaires/numériques seulement. Aucun `tr2_validate.sh`, build hôte/ARM, test PC, ST-LINK, GDB, flash/reset, Modbus, accès FRAM/microSD/lecteur PC. Les 106 cas sont spécifiés, non exécutés ; les résultats matériels M014/M015/SD restent ceux des archives. L'état physique des deux cartes est hérité et non relu, aucun firmware ou média restauré.

## Risques ouverts et conditions R1

Aucune contradiction normative identifiée à l'issue de la revue locale. Restent des **préconditions et preuves d'implémentation**, pas des encodages laissés à décider :

1. Disponibilité/attribution réelle de la zone FRAM avant toute écriture ; contenu inconnu→arrêt, jamais « libre car non référencé ». Rollback seulement vers cumulatif compatible V2, pas pré-V2.
2. Port de l'enveloppe logique 3, nouveau décodeur identité/autorité, migration et coupures sur vrais stores ; conserver strictement les anciennes banques, UID et données. L'ancien UNAVAILABLE ne doit plus masquer la candidate LIVE autoritative.
3. Implémentation B5 epoch et rétention actuelle limitée ; slots 126/127 réservés STOP/renouvellement, sans GC d'identité admise. Après STARTED sans reçu d'effet, recovery 9 peut réellement bloquer les commandes ; aucun reset ou timeout n'est une preuve d'absence autorisant 28/29. Prévoir diagnostic et résolution fondés sur preuves, pas de blind replay.
4. Coût des commits complets de 52 Ko, SRAM/pile et ordonnanceur IO compatibles FIFO ; aucune marge temps réel déduite du simple calcul géométrique. Le chemin STOP ne doit pas subir de famine, à démontrer en R4.
5. Qualifier plus tard firmware, PC et physique selon R1–R6 ; R7 stockage reste distinct, sans bloquer LIVE. L'absence de mesure physique R0 est conforme au contrat, pas une qualification positive.

Entrée R1 : contrôle indépendant de ChatGPT des documents/mapping/oracles et nouvelle mission explicite. Roadmap R0 actualisée « documents rédigés, contrôle ChatGPT à réaliser », sans qualifier R1/J3/J4/J5. Journal complété factuellement ; dettes A04, gels, contrat R0 et règles de gouvernance intacts.

## Git et publication

Dépôt original propre à `82169af7e76797731358852bec83bfb0ceb54ccf`, divergence historique conservée ; aucun pull/merge/rebase/reset destructif. Stashes `6251b988e8b20f97997a8f98fd26d4d35a45a1b2` et `f3b829e6188f491920edf74a5f9ed0f559d08b87` conservés. Worktree `/home/lolo/dev/msm/projects/MSM_vibe-R0-live-successor`, branche locale `codex/r0-live-successor`, publication autorisée vers `refs/heads/main` uniquement.

`result_sha` désigne la référence initiale statiquement examinée, pas un code exécuté. Seuls nouveaux documents successeurs/preuves, archive A06, rapport, journal et statut factuel de roadmap sont inclus. Aucun fichier applicatif modifié. Commit/push autorisés exercés ; flash/debug false respectés. Diff et index examinés, contrôles documentaires réussis. Premier commit **`61554e5616c7d08a2f177bd1ed361b798dd6b094`**, message `R0: specify continuous LIVE profile, safe FRAM migration and successor validation`, poussé normalement sur main. Après `git fetch origin main`, les **22 livrables ont été relus intégralement et comparés octet à octet**, 7 empreintes de preuves vérifiées et 2 940 fichiers protégés contrôlés également dans les objets distants ; stashes conservés. [Reçu distant](preuves/TR2-20261015-R0/remote-readback-61554e5.json).

La présente clôture ajoute le reçu et consigne le statut DONE ; elle est à son tour poussée normalement et relue. Son SHA final est fourni dans la réponse sans auto-référence. Le contrôle indépendant ChatGPT et les autorisations R1 restent distincts ; aucun jalon logiciel/physique n'est qualifié par ce statut. Worktree propre au premier checkpoint, uniquement la clôture documentaire ajoutée ensuite.
