---
mission_id: TR2-20261015-A06-DECOUPLAGE-SUPERVISION-STOCKAGE-AUDIT
status: DONE
base_ref: main
base_sha: 0c88e6ef195b44fdbf21890d946ef7ea8bf190fe
initial_head: 45ee898a6ebe9269e63904262dc9f1b9c8b87c78
result_sha: 45ee898a6ebe9269e63904262dc9f1b9c8b87c78
created_at_utc: 2026-10-15T09:01:52.720065+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# A-06 — Découplage faisable, avec extension explicite du contrat

**Étude terminée, publiée sur main et intégralement relue depuis la référence récupérée de GitHub.** Le mode supervision sans média de campagnes est faisable en réutilisant acquisition, calculs et projections existants. La V1 actuelle l'interdit au niveau configuration et impose une ouverture de campagne au START : supprimer un test de capacité ne constitue pas une solution conforme.

**Recommandation ferme : extension versionnée avec configuration live autoritative en FRAM et commandes live distinctes.** Préserver les commandes 3/4 de campagne et leurs oracles, ne pas inventer de capacité ou de magasin donnant un faux succès. Le stockage industriel reste prévu et sera qualifié/réintégré séparément. La décision utilisateur de découpler est acquise ; seuls les encodages et politiques résiduels nécessitent le travail normatif de ChatGPT.

[Étude autonome complète](../ETUDE_A06_SUPERVISION_SANS_STOCKAGE.md) : constats, proposition précise, compatibilité/migration, matrice des FT/TT et trajectoire L0–L7. [Matrice exigence → test → code → impact → proposition](preuves/TR2-20261015-A06/traceability.csv), 20 relations avec chemins vérifiés. [Classement des 2 167 fichiers](preuves/TR2-20261015-A06/file-impact-screening.csv) et [inventaire empreinté](preuves/TR2-20261015-A06/corpus-inventory.csv).

## Tranches et constats

| Tranche | Résultat réel |
|---|---|
| Référence/Git/gouvernance | Mission READY 45ee898, base 0c88e6e ancêtre ; règles, protocole, journal intégral, roadmap et précédent CN8 examinés. Dépôt historique divergent préservé, worktree neuf depuis main distant. |
| Corpus spécification/validation | Lecture automatisée intégrale de 2 167 fichiers/3 524 702 octets ; revue des contrats et familles actives. 10 sources normatives,2 157 éléments de validation dont 1 530 historiques ; FT-ACC-01 retirée. 129 copies binaires surnuméraires identifiées, sans fusion. |
| Dépendances firmware | Validateur exige mode 1 / limite positive/capacité connue ; recovery UNAVAILABLE ne publie pas d'active. CampaignService ouvre dépôt + DataStore avant acquisition. Acquisition/calcul ont déjà des responsabilités distinctes, intégration FIFO→B3 à achever. |
| PC/SQLite | B3ArchivePublisher ne requiert pas de campagne B6 ni de SD ; session compatible nécessaire. PC actuel refuse une version différente par égalité stricte. SQLite archive des observations B3, pas le flux XYZ ni les trous hors connexion. |
| Étude normative | Trois stratégies comparées ; recommandation versionnée, tuple live 2/0/0 proposé uniquement pour successeur, commandes candidates 13/14 (12 déjà alloué V1.1), migration FRAM et CRC traités. Aucune proposition adoptée/implémentée ici. |
| Validation | Contrôles documentaires, chemins, empreintes, archives et préservation ; aucun build/test firmware/PC ou essai physique rejoué. Relecture distante des 17 livrables effectuée au commit 67c074e ; aucun changement des fichiers protégés. |

Le parcours intégral est traçable par fichier, titres actifs et occurrences de dépendances ; il ne signifie pas une lecture humaine individuelle ni une qualification de chaque fiche générée. Le classement par famille dans `file-impact-screening.csv` est conservateur, accompagné de la revue détaillée de l'étude. Répartition : 1 869 inchangés, 7 gels impactés, 256 à compléter, 35 à modifier pour le successeur ; **les fichiers actuels restent intacts**. Les nouveaux tests sont proposés dans l'étude et ne sont pas comptés comme existants.

## Matrice technique synthétique

Chemins complets, exigences/TT et propositions détaillées : [traceability.csv](preuves/TR2-20261015-A06/traceability.csv), A06-01..20.

| Exigence → couverture | Code réel / impact / proposition |
|---|---|
| B4 domaines → FT-LIM01/02, LIM01-RQ008/012/013, LIM02-C02 | configuration_validator.c : gel impacté ; mode campagne inchangé, nouveau tuple live seulement en version nouvelle ; capacité réelle exigée pour campagne. |
| B4 CRC/miroirs/apply → FT-LIM03, TT-INT-B04B05-001..004 | b4_configuration_codec.c/configuration_service.c : garder 168/152 octets et vecteur 5207CCFC ; nouveaux vecteurs et contrôle schéma hors CRC ; commit avant active. |
| Recovery/autorité → FT-PER03/04/06 | configuration_store.c/configuration_record.c : nouveau format refusé par ancien firmware, lecture explicite anciens records ; aucune conversion automatique campagne→live. |
| B5 START/STOP → FT-CMD06, FT-LIM04, TT-INT-B05B06-001/002 | campaign_service.c : préserver 3/4 et ouverture/clôture ; nouvelles commandes live sans ID campagne ni DataStore. Epoch V1.1 conservé distinctement. |
| B1/B3/B7 → FT-LIM08/10, TT-BLK-B03-201..209, TT-INT-B03B04-001..004, FT-INT05 | projection.c/supervision_service.c : mesure valide indépendante du média, âge réel et défauts visibles ; bit 8 et synthèse des états à spécifier. Calculs d’accélération mg inchangés, raccordement et qualité encore à développer. |
| B6 inventaire → FT-BLK05, FT-LIM05/06, TT-SEQ-CAMP-001/002 | b6_projection.c : aucune campagne fictive ; indisponible distinct de vide, nouveau statut 4 proposé exclusivement dans le successeur ; historique préservé. |
| Structure/accès → FT-STR/ACC | codec.c/write_adapter.c : adresses/types/endian/RO/RW conservés ; mapping successeur distinct après décision, sans modification des gels. |
| PC/SQLite → PollingB3ArchiveIntegrationTests/SqliteB3ArchiveSinkTests | B0SessionFactory/B3ArchivePublisher/SqliteB3ArchiveSink : refus de l’ancien PC sûr, reconnaissance explicite future ; métadonnées version/config/mode et gestion trous/erreurs à compléter. |

Les NOT_DEFINED actuels demeurent : dérivation exhaustive états/validité/alarmes, dominance, hystérésis/hold, priorités de refus simultanés et certaines relations inter-blocs. Ne pas créer de PASS depuis des annexes métier informatives ou supposer une égalité B1 active_campaign_id/B6 universelle. L'étude propose une sémantique live explicite, sans la rétroattribuer à V1.

## Ordre recommandé, risques et arbitrages

1. ChatGPT contrôle cette étude puis publie l'extension normative : versions/capacités, tuple live, codes 13/14, bit 8, B6 indisponible, politique autotest/défauts, migration/rollback et contrat transactionnel V1/V1.1 retenu.
2. Implémenter autorité FRAM/recovery puis orchestration live et fenêtres/calcul/B3, avec refus sûrs et propriétaire FIFO unique. Quatre fenêtres V1, échelle réelle, snapshots cohérents ; préserver les dettes et marges A04.
3. Validation complète firmware/ARM et tests PC concernés, puis **nouvelle mission physique autorisée** capteur→B3→PC→SQLite, sans média, avec budgets et durées chiffrés. Ne pas considérer M014/M015 comme preuve de cette nouvelle chaîne.
4. Réintégrer l’enregistrement après qualification média distincte, même flux capteur, backpressure bornée, panne/plein/retrait explicitement signalés et préfixe durable vérifié ; retour média sans auto-ouverture. Ne pas risquer la partition existante.

Risques principaux : adoption implicite du mode 0 V1, capacité inventée, changement invisible START, version hors CRC, downgrade FRAM illisible, faute SD masquée, B3 zéro ou périmé affiché valide, I/O FRAM bloquant FIFO et confusion SQLite/campagne. Le mode interne temporaire ne satisfait pas la supervision d'exploitation ; il ne doit pas créer une dette de conformité cachée.

## Preuves antérieures et limites matérielles

[M016 bloqué](archives/TR2-20261013-M016-V1-VIBRATION/RAPPORT.md) et sa cartographie/probe sauvegardé réutilisés sans réexécution. M014-H ne prouve que polling B0/B1/perte/reprise bornés. M015-V (`ef468539...`) :32 768 XYZ / 4 096 TS / ~1,247 s et deux lectures B0/B1, pas B3 métier. A04-02 (`8bcb843...`) : gap 5,855 ms / marge nominale 1,445 ms et B5 refusé pendant capture, I/O au repos 144,138 ms et boot bloquant restent des limites. Le rapport CN8 et sa mission sont archivés byte-identiques depuis 0c88e6e, sans effacer les preuves FIRSTFAULT.

**Aucune opération matérielle A-06** : ni ST-LINK/GDB/flash/reset/Modbus/RS485, ni accès lecteur PC/FRAM/microSD. État uniquement hérité, non relu : principal dernier probe STORAGE2, cumulatif A04-02 non restauré ; seconde probe FIRSTFAULT B terminal 99 / trigger 0 / run 1 / error 2, SD test insérée / STLINK seul / CN15 off selon dernier retour utilisateur. Aucun nouveau résultat physique ou restauration revendiqué. Contrôle indépendant de ChatGPT distinct du statut DONE de l'audit.

Journal actualisé par une section factuelle A-06 : décision du 15/10, dépendances observées, recommandation non adoptée, aucun matériel accédé et suite normative. Roadmap et dettes restent inchangées ; M016/A04-03 ne sont pas clôturées.

## Git, contrôles et publication

HEAD initial distant 45ee898a6ebe9269e63904262dc9f1b9c8b87c78. Dépôt original `/home/lolo/dev/msm/projects/MSM_vibe` propre à 82169af7e76797731358852bec83bfb0ceb54ccf, divergence historique non résolue et tous les worktrees préservés. Worktree A06 `/home/lolo/dev/msm/projects/MSM_vibe-A06-supervision-storage-audit`, branche locale `codex/a06-supervision-storage-audit`, publication explicite vers `refs/heads/main`. Stashes 6251b988e8b20f97997a8f98fd26d4d35a45a1b2 et f3b829e6188f491920edf74a5f9ed0f559d08b87 conservés.

Livrables : présente étude et rapport, preuves d'audit, archives CN8 intactes et ajout journal factuel uniquement. Code, spécifications, FT/TT, mappings, architectures normatives, gouvernance/contrat/roadmap/dettes protégés par comparaison empreintes. `result_sha` désigne le code et le corpus **inspectés statiquement**, pas un code exécuté ou flashé par A-06. Permissions commit/push seules exercées ; flash/debug false respectés.

Commandes réalisées : git status/rev-parse/fetch/ls-remote/worktree list/stash list, inspections rg et lectures, inventaire Python des 2 167 fichiers, génération matrices, vérification des chemins et archives, contrôle empreintes des 2 939 fichiers protégés. Validation proportionnée documentaire : aucun changement fonctionnel, donc aucun test/build hôte/ARM/PC ni reprise essai SD ne fournirait une preuve de plus pour l'étude. Les contrôles de livrables ne sont pas des tests fonctionnels. Le contrôle initial du diff indexé a signalé les fins de ligne CRLF des nouvelles preuves CSV ; sérialisation corrigée en LF avec champs entre guillemets, sans modification des sources ni des archives.

Publication normale sur `main` réussie au commit **`67c074ed5cb0fcbdee29de373f1c2c4d4e9af5c5`**, message `A06: audit live supervision without campaign storage and preserve V1 contracts`. Référence récupérée par `git fetch origin main`, puis **17 fichiers intégralement relus et identiques octet à octet**, 11 empreintes de preuves vérifiées et 2 939 fichiers protégés contrôlés également dans les objets distants. [Reçu de relecture](preuves/TR2-20261015-A06/remote-readback-67c074e.json). Dépôt historique et deux stashes préservés ; worktree propre au checkpoint.

La présente clôture `DONE` consigne ce résultat et ajoute le reçu ; elle est ensuite poussée normalement et relue à son tour. Son SHA final sera fourni dans la réponse, sans auto-référence impossible. Aucun merge/rebase/reset/clean/force push ni résolution automatique de divergence. `DONE` qualifie la livraison de l'étude, pas le mode live, la V1, le matériel ou une décision normative. Contrôle indépendant ChatGPT et mission d'implémentation restent à réaliser.
