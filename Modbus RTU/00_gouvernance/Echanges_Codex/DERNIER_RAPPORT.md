---
mission_id: TR2-20261015-R2-LIVE-COMMAND-ENGINE
status: DONE
base_ref: main
base_sha: f56d0a688e520d556965ae78e94d30203d5dab0f
initial_head: 091f5e8f22cb500c6f331df325da60315d59a4d7
result_sha: 59d3757e88c6fadbc36a2de1cb17c6b67a15186a
created_at_utc: 2026-10-15T10:59:16Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R2 — moteur epoch / commandes LIVE, livraison logicielle et limites

**DONE — livraison du moteur logiciel, preuves publiées et relues ; limites de composition maintenues.**
Le journal epoch, les commandes12/13/14, APPLY1 et l'adaptateur B5 dédié29registres
fonctionnent sur vrais stores avec transport et acquisition simulés. **La
composition STM32/protocole2 complète reste non raccordée.** Aucun firmware LIVE
opérationnel, résultat B3 ou qualification physique n'est revendiqué. Le critère
contractuel « implémentés ou limites d'intégration honnêtement exposées » est
traité explicitement ci-dessous ; la clôture de livraison ne vaut pas intégration
complète de tous les objectifs produit R2–R6.

## Références et préservation

Mission READY récupérée sur `main` au HEAD initial
`091f5e8f22cb500c6f331df325da60315d59a4d7`, base contractuelle `f56d0a6` vérifiée
ancêtre. `created_at_utc` absent du contrat, aucune date contractuelle inventée.
AGENTS.md, protocole, gouvernance §10, journal intégral, contrat R0, profil,
migration, mapping, tests/oracles et rapport R1 consultés avant développement.

Dépôt principal propre à `82169af7e76797731358852bec83bfb0ceb54ccf`, historiquement
divergent, préservé sans pull/merge/rebase/reset. Nouveau worktree
`/home/lolo/dev/msm/projects/MSM_vibe-R2-live-command-engine`, branche locale
`codex/r2-live-command-engine`, destination autorisée `main`. Worktrees et deux
stashes `6251b988e8b20f97997a8f98fd26d4d35a45a1b2` /
`f3b829e6188f491920edf74a5f9ed0f559d08b87` conservés. **2382 empreintes protégées**
recomparées, identiques (gels, corpus R0, archives, gouvernance/mission, dettes,
environnement et expériences sélectionnés). Aucun fichier ancien hors des trois
points de raccordement R1/CMake modifié dans le firmware. Mission et rapport R1
archivés intégralement depuis `f56d0a688e520d556965ae78e94d30203d5dab0f`, sans
écrasement des archives A-06/R0.

## Code livré

Commit évalué : **`59d3757e88c6fadbc36a2de1cb17c6b67a15186a`** —
`R2: implement durable epoch command engine and LIVE effect barriers`.
Après ce commit, seuls preuves, rapport, journal et roadmap évoluent.

- `LiveCommandStore` : journal4/TR2Q/TR2E aux offsets R0, CRC/formats/réservés,
  sélection des copies, identités, phases et conflits ; aucun namespace V1 partagé.
- `LiveCommandEngine` : admission versionnée, retry/collision avant contexte,
  cycle STARTING/LIVE/STOPPING/READY/FAULT, STARTED avant effet, reçu causal avant
  COMPLETED, arrêt de sécurité indépendant des écritures FRAM.
- Renouvellement N→N+1 explicite, pending lié au journal, retry sans N+2, plafond
  sans wrap. Slots0..125 ordinaires,126 STOP,127 renouvellement ; aucune éviction
  courante. Réserves numériques de générations, en plus du budget I/O obligatoire.
- Run_id réservé dès RESERVED et conservé après abandon. Recovery28 avec barrière
  avant effet,29 seulement avec preuve durable causale explicite, sinon9 sans
  redispatch. Boot arrête le capteur via son port même si FRAM indisponible ; un
  arrêt au boot n'est jamais utilisé comme preuve d'absence d'effet antérieur.
- Mailbox immuable, accès B5 dédiés29registres, RO refusé avant mutation, clear
  incluant epoch, last autoritatif et timestamp historique (zéro valide inclus).
- APPLY1 raccordé au vrai service R1, préparation capturée et refus en acquisition
  avant I/O. Le lecteur complet de contrôle R2 est attaché explicitement avant
  montage ; le comportement strict R1 reste le défaut, sans option permissive.

Architecture et conditions d'intégration :
[`LIVE_R2.md`](../../05_Firmware/LIVE_R2.md). Le code ne contient aucun port SD,
DataStore ni allocation de campagne ; le test d'acquisition reste un modèle hôte.

## Limites de composition — éléments non réalisés

| Élément | Fait vérifié / prochaine action |
|---|---|
| Production STM32 et RTU | Toujours protocole1, B5 historique20registres ; nouveau backend non appelé par la composition. L'adaptateur29registres est testé directement, pas à travers un FC03/FC10 physique. |
| Acquisition réelle | Port causal défini, sans implémentation STM32 continue installée. Le FIFO coopératif existant est une capture bornée de qualification, avec refus d'I/O pendant capture ; ce n'est pas le propriétaire continu attendu de R3. Sans port complet, START refuse avant admission. |
| B0/B1/B4/B6/B7 successeur | Identité R1 préparée sans bitLIVE et projection B4 disponibles ; pas de publication protocole2. États/projections système, inventaire et diagnostics successeur restent à composer avec leurs autorités réelles. |
| Commandes héritées2..11 | Production V1 conservée ; effets successeur non raccordés, refus5/3 sans admission après contrôles. Aucune réutilisation silencieuse du journal V1 ni succès inventé. |
| B3 / R3 | Aucun calcul ni snapshot valide simulé. Compléter propriétaire FIFO/configuration et calcul, puis raccordement cohérent du profil avant annonce de capacité. |
| R4 / R6 | Volumes synchrones mesurés dans le modèle seulement ; aucune marge FIFO/Modbus ni preuve physique acquise. Pas de provisioning matériel LIVE. |

Ces limites sont exposées conformément au contrat ; **R2 est reçu ici au niveau
moteur logiciel, pas comme une composition successeur complète**. Le contrôle
ChatGPT doit décider de la tranche complémentaire de raccordement à inclure avec
R3, notamment effets hérités et projections système. Aucun changement d'architecture
normative, valeur réservée ou gel historique appliqué pour masquer ces limites.

## Validations réellement exécutées

Preuves : [`TR2-20261015-R2`](preuves/TR2-20261015-R2/README.md), logs bruts,
empreintes des fichiers évalués, ressources ARM et manifeste de preuves.

| Validation | Résultat et portée |
|---|---|
| `STM32CUBE_U5_ROOT=.../STM32CubeU5-v1.9.0 bash ./tr2_validate.sh` | Premier cycle complet PASS hôte+ARM ; validation complète justifiée par interfaces/cœur partagés. |
| Rebuild final hôte puis `ctest --test-dir .../build-host-validation --output-on-failure` | **130/130 PASS**,38,05s ; inclut R1 et tous les oracles historiques, après la dernière correction du flag acquisition pendant STOPPING. |
| Test R2 sur vrais stores | 384 décisions R0 copiées de CSV indépendant ; flux réalisables sur médias réels simulés,126 admissions, STOP/renew réservés, retry/collision/reboot/epoch et temps. Pas de prétention à384 campagnes matérielles. |
| Injection power-loss R2 | **1561 budgets**,10 étapes :9×142 +283 pour APPLY à deux commits. Payload par pas4096, tous octets headers/publication ; historique et tail byte-identiques, aucun redispatch/auto-START à chaque cas. Pas chaque octet du payload ni atomicité électrique. |
| Fautes et recovery | Capteur non confirmé, FRAM défaillante,28/29/9, reçu non causal refusé, renouvellement pending/indéterminé, epochFFFFFFFF synthétique, réserves et run_id non réutilisé ; aucune fausse réussite. |
| ASan/UBSan | Build/exécution finale du test R2 avec `-fsanitize=address,undefined -fno-omit-frame-pointer` : **PASS**, aucune erreur rapportée. |
| Cross-build final | `bash ./tr2_validate.sh --cross-build-only` avec SDK Linux qualifié : **PASS**. Fonctions R2 compilées, non référencées par le cumulatif V1. |
| Lien ARM ciblé | ELF de contrôle sans board/startup/map production, symboles moteur/store/R1 effectivement liés : **PASS**, jamais destiné au flash ni exécuté. Source/commande/SHA dans preuves. |

**Défaut détecté puis corrigé :** le nouveau test de renouvellements consécutifs
perdait last après écrasement de ses deux copies par le nouveau non-terminal.
La copie portant le dernier terminal est maintenant retenue jusqu'au nouveau
terminal, grâce à la transaction d'image atomique ; test conservé, log d'échec
préservé. Aucun oracle historique modifié, aucun warning désactivé, aucun test
supprimé. Les relances finales sont justifiées par cette correction et celle du
flag acquisition à maintenir pendant STOPPING, pas par une campagne physique
répétée. Les logs ciblés intermédiaires ne sont pas tous ceux du SHA final ; les
logs `host-final`, `arm-final`, sanitizer et extrait LastTest portent le résultat
final dont les fichiers sont empreintés.

Binaire cumulatif **non flashé**, SHA256
`ac27b274bf526ba71c363fd1c331bb5e82048d8f19c192a601526560f759cbea`, identique au BIN
R1 conservé ; ELF `b0c983425c516beeffe8baf748419c87b19785cffa0e8ff44fd3ffb61ea9bfb9`.
Le BIN identique ne prouve aucune intégration LIVE : il confirme que le backend
n'est pas lié à la composition V1. ELF de lien R2 non flashable :
`b5111e23e80d58baccf4a415407f028f7a76ce5332b708c65d53f03359de54f0`.

## Ressources et coût — aucune marge physique inventée

ARM GCC14.2.1 Cortex-M33 -O0 : moteur480, store9656, config584, média96,
candidate52298 octets. Design autonome63210octets hors alignement/callbacks,
**+10144** sur R1. Cadres `.su` : submit440, step400, commitstore160 ; maximum440
parmi les unités analysées, pas la profondeur complète avec HAL/IRQ ni le
high-water mark. Aucun objet R2 instancié dans la RAM de production actuelle.

START nominal du modèle : **4 commits,209704octets écrits,1255664lus**. Chaque
commit écrit52426octets ; STOP nominal4 commits, APPLY5 commits, renouvellement4.
Le découpage par étapes conserve la causalité mais ne rend pas les appels FRAM
préemptibles. Le callback budget absent/refusé empêche l'admission ; un vrai
ordonnanceur R4 doit garantir STOP/renew sans famine, durée des relectures/hash/CRC
et marges FIFO. Les réserves numériques ne qualifient pas cette disponibilité.

## Matériel, documents et publication

**Zéro opération matérielle** : aucun flash/debug/reset, accès FRAM/SD ou trafic
RS485. Propriété/contenu réels de [122880,245760) non établis, aucun fournisseur
matériel d'autorisation créé, aucune écriture tentée. Firmware/cartes/médias dans
leur état hérité **non relu**, sans restauration M014-H supposée. Autorisations
commit/push exercées ; flash/debug non exercées. HP retiré ; futur banc T1000 à
inventorier avant tout usage. Option Bytes/câblage/protections non touchés.

Journal et roadmap actualisés : réception logicielle R1 selon contrat R2, moteur
R2 testé, points non raccordés, coût R4 et prochaine intégration explicités.
Dettes A-04, gels V1/V1.1, corpus R0 et preuves antérieures préservés.

Premier push normal réussi au commit
**`b78e7105a9a7da7ed6ba6fe3d57b0bfbc6cf0204`** —
`R2: publish engine validation evidence and explicit integration limits`.
Après fetch, **45 fichiers livrés intégralement relus depuis cette référence
GitHub et byte-identiques**, rapport provisoire compris ;29 empreintes de preuves
et2382 fichiers protégés contrôlés. Stashes inchangés, worktree propre avant
ajout du reçu/clôture. Preuve versionnée :
[`remote-readback.json`](preuves/TR2-20261015-R2/remote-readback.json).

Destination `https://github.com/loloLR17/MSM_vibe.git`, push normal
`HEAD:refs/heads/main`, sans force ni résolution automatique d'une divergence.
Le rapport passe à DONE pour la livraison logicielle et ses limites contractuelles,
sans annoncer la composition complète ni une qualification indépendante.
La publication finale est contrôlée à son tour ; son SHA et l'état Git final
sont fournis dans la réponse Codex, sans autoréférence inventée dans ce commit.
Le dépôt principal, les travaux historiques et les médias restent préservés.
