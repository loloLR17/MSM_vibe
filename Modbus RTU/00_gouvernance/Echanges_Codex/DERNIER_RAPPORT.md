---
mission_id: TR2-20261017-R5A-PC-LIVE-PROFILE-SQLITE-WEB
status: BLOCKED
base_ref: main
base_sha: d0a37efb7f29d6fb8ff4e275e6f666916ff79abb
initial_head: 8f16632ea3595b7421d9a7373d33c88bd269cbc4
result_sha: 1efc4f1500f8401a5996951f8798a8c977019ec2
created_at_utc: 2026-10-17T11:26:58.376226+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# R5-A — tranche PC livrée, conformité B3 physique bloquée

**Blocage réel : le firmware LIVE inverse ALARM_FLAGS et SEVERITY_GLOBAL aux adresses 3002/3003.** Le lecteur normatif refuse donc les mesures des captures R4-U/V. La correction STM32 sort du contrat PC, qui interdit flash/debug ; aucun échange automatique des mots côté PC ni changement de norme n'est appliqué. R5-A ne satisfait pas tous ses critères et n'est pas déclarée DONE. Les tranches logicielles réalisables ont été développées et testées.

## Constats et preuve du blocage

Le mapping LIVE hérité et le décodeur PC existant imposent 3002=alarmes, 3003=sévérité. `Modbus RTU/05_Firmware/src/application/live_metrology.c:203` écrit la sévérité en `registers[2]`, et la ligne204 écrit les alarmes en `registers[3]`. Ce fichier à la référence initiale est identique à celui exécuté en R4-V (`8db2f67`).

La première valeur du candidat R4-V, `10:36:09.3891128 UTC`, contient réellement 3002=3, 3003=240, statut4/flags1215, séquence1, RMS958mg ; CRC valide. **240 est une sévérité réservée.** Sur 4362 ADU réanalysées sans aucun essai physique : 2217 B3, dont2068 avec cette sévérité non conforme (780 R4-U,790 baselineR4-V,498 candidatR4-V). Les149 autres sont des états sans mesure valide. Le PC conserve les octets/décodage pour diagnostic, jamais comme résultat qualifié.

Les mesures FIFO/latence/FRAM historiques ne sont ni rejouées ni supprimées. Elles ne démontrent pas la conformité intégrale des champs B3. Les archives R4-U/V restent intactes. [Premier/dernier exemple et comptes](preuves/TR2-20261017-R5A/wire-mismatch.json), [provenance source/mapping](preuves/TR2-20261017-R5A/source-provenance.json).

## Modifications réalisées

Le commit logiciel **`1efc4f1500f8401a5996951f8798a8c977019ec2`**, `feat(r5a): add strict readonly LIVE observations and SQLite Web integration`, ajoute une voie de lecture au socle existant, sans refonte et sans nouveau logiciel séparé. [Note technique, matrice des champs et reprise](../../Supervision%20TR2/LIVE_R5A.md).

- Sélection exacte V1/protocole1 ou LIVE/protocole2, vérification des tailles, versions B4/B6/B7 et capacités ; rejet des versions inconnues. Les images partielles sans LIVE16 restent refusées par défaut et accessibles seulement par un opt-in LiveDiagnostic explicitement étiqueté. Aucune inférence epoch sur V1, aucune écriture B4/B5 ni lecture B5 longue.
- Décodage normatif B3/48 avec MSW/LSW, mg, âge/séquence, réservés, V/F/HELD, UTC et cohérence de sévérité. Une trame refusée n'obtient pas de snapshot validé ; registres et décodage normatif sont conservés en quarantaine. Les alarmes/sévérité réelles ne sont pas réinterprétées silencieusement.
- Observations avec identité, profil/versions/capacités, bus/adresse, source, réception PC et segment. UTC fenêtre déclarée par B3 distincte de la réception PC et de toute vérification B2. Run/epoch/configuration/génération/temps événement non exposés : null, aucun campaign_id inventé.
- Séquences répétées, trous, saturation, recul, reconnect et restart distingués ; nouveaux segments PC, sans preuve de boot ni exactly-once prétendue. Erreurs transport, rejet et erreur d'archivage visibles ; SQLite indisponible ne devient pas un succès et le cycle suivant peut reprendre.
- Migration7 additive vers live_observation, sans modification des anciennes lignes b3_archive ni du journal B5. Tests sur une structure6 contenant une ligne historique avant migration, puis observations LIVE distinctes et répétées conservées.
- GET `/api/v2/live` et `/live.html` intégrés au host S7, sans action d'écriture. HELD ou réception périmée ne deviennent pas une valeur courante ; UTC absente, qualité, continuité, source et archivage sont affichés. Contrats V1 conservés. Mode explicite `--live-replay` utilisant les mêmes couches et des captures historiques, sans port série.

Le replay utilise B0/B3 authentiques contrôlés par CRC ; ses headers B4/B6 absents des captures sont **simulés et annoncés comme tels**. Le timestamp de relecture PC n'est pas celui de l'acquisition historique. Ce mode ne prouve ni la présence actuelle d'un firmware LIVE, ni R6.

## Validations et incidents conservés

Environnement T1000 Debian WSL2, SDK.NET10.0.401/runtime10.0.12. Commandes détaillées dans la note et les logs :

| Validation exécutée | Résultat |
|---|---|
| dotnet restore de la solution | PASS |
| dotnet build --no-restore | PASS,0warning/0erreur |
| dotnet test de la solution --no-build | **551 PASS/0skip**,539 anciens +12 nouveaux cas |
| Après dernière correction Web/fixtureUTC : tests Service | **201 PASS**, compilation dépendante réussie |
| Tests UI Node avec DOM simulé | **7 scénarios PASS**, pas de navigateur graphique qualifié |
| Démarrage réel service/HTTP hors ligne | Routes `/`, `/live.html`, `/live.js`, `/api/v1/fleet`, `/api/v2/live` :200 |
| SQLite du smoke-test |52observations, dont1quarantainéeV/F0 ; export conservé |
| Non-écriture / compatibilité historique |PASS sur transports simulés ; aucun accès COM réel |

Les tests couvrent sélection stricte, CRC/longueur, endianess, qualités/réservés, UTC absente, vrais ADU non conformes, gaps/repeats/saturation/reboot/reconnect/restart, corruption/indisponibilité SQLite, ancienne base et contrats API/S7. Le code firmware n'est pas modifié : aucun build ARM ou essai STM32 inchangé n'a été répété.

Échecs réels conservés : premier chargement hex incompatible avec les espaces du captureur, corrigé sans changer les octets ; puis les3tests de captures ont révélé le défaut firmware. Leur oracle final exige le **refus** des trames, pas une acceptation de240. Les fixtures normatives sont indépendantes. Le premier smoke-testHTTP a échoué404 : fichiers statiques cherchés seloncwd ; correction ciblée de ContentRoot/WebRoot vers le répertoire du binaire, où les fichiers sont déjà copiés. Les201tests Service et le second smoke-test passent après correction. Les logs initiaux restent publiés.

## Preuves et publication

[Manifeste SHA-256](preuves/TR2-20261017-R5A/SHA256SUMS), [résumé HTTP/SQLite](preuves/TR2-20261017-R5A/smoke02-summary.json), [API non conforme](preuves/TR2-20261017-R5A/smoke02-api-live.json), [tests complets](preuves/TR2-20261017-R5A/test-final.log), [tests Service finaux](preuves/TR2-20261017-R5A/service-final.log), [préservation](preuves/TR2-20261017-R5A/preservation.json). TRX compressés, sources de vérification, export SQLite temporaire et journaux d'échecs sont conservés avec SHA brut/stocké. Rapports opérationnels : `/home/lolo/dev/msm/reports/TR2-r5a`.

Travail dans `MSM_vibe-R5A-pc-live`, branche `codex/r5a-pc-live`, créée depuis le main distant vérifié `8f16632ea3595b7421d9a7373d33c88bd269cbc4`. L'ancien main local82169af reste propre et intact.7855fichiers protégés (firmware, normes, archives et preuves antérieures),63autresworktrees, étudeR4-S86d0f4b et2stashes6251b988…/f3b829e6… sont vérifiés inchangés. Aucun merge/rebase/reset/forcepush ni secret inclus. Commit/push normaux autorisés exercés ; flash/debugfalse respectés.

Journal actualisé avec le blocage de conformité, la tranche PC testée et l'absence d'intervention matérielle ; roadmapR3/R5 factualisée. Les décisions normatives et la detteA04 ne sont pas modifiées. L'état matériel M016/FRAM70e273…73587 reste le **dernier état rapporté R4-V**, non une nouvelle observation.

Ce bilan est préparé pour publication normale surmain et relecture distante ; le statutBLOCKED décrit l'obstacle firmware, pas l'attente GitHub. La preuve de publication et les SHA relus seront ajoutés après le push.

## Reprise attendue de ChatGPT

Publier un contrat autorisant la correction ciblée de `live_metrology_project_b3` et sa validation : alarmes à3002, sévérité à3003, oracles indépendants de l'encodeur et contrôle d'une capture réelle incluant V/F/UTCabsente/HELD. La norme ne doit pas être changée pour suivre le défaut. Les permissions firmware/flash/debug manquent volontairement à R5-A ; elles doivent être accordées dans ce contrat avant ces opérations.

Les acquis PC sont reprenables au SHA ci-dessus. Ensuite, connecter le lecteur au scheduler série unique R5-B, identifier le vrai firmware/capacités, et qualifier la chaîne SQLite/Web réelle. B1/B2/B7 et produit LIVE complet restent à composer ; aucun GO R5/R6 ou industriel, aucun tag ni qualification indépendante ChatGPT attribuée par Codex.
