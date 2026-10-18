---
mission_id: TR2-20261017-R5B-LIVE-PHYSICAL-TO-WEB-DEMO
status: BLOCKED
base_ref: main
base_sha: ae24fa3c5a2773f9379b021f3457513d844545ef
initial_head: 868a80dcba90f794aa694cc470fca6a421292a2e
result_sha: dc773733a98d78016e1d0f8a341b155e1851e2c7
created_at_utc: 2026-10-18T16:22:56.929982+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R5-B — checkpoint logiciel préservé, campagne bloquée par Windows

**BLOCKED : la chaîne physique COM4→service→SQLite→Web n'a pas été démontrée.** Le vrai service Windows est refusé au chargement de `TR2.Protocol.dll` par Code Integrity (0x800711C7, événements3033/3077). Une voie de déploiement autorisée/signature adaptée doit être définie par le responsable du poste. Aucune protection n'a été modifiée ni contournée. Le blocage n'est pas un échec technique du capteur ou du Modbus.

## Acquis publiables

Commit logiciel **`dc773733a98d78016e1d0f8a341b155e1851e2c7`**, `feat(r5b): prepare bounded readonly physical LIVE service checkpoint` : mode direct du même service, transport NModbus existant, opt-in diagnostic strict, trace d'octets sous NModbus, connexion unique sérialisée/fermeture/reconnexion, API/Web/SQLite7 réutilisés. Aucun faux header réel, writer de commande ou FC10 automatique ; ancien profilV1 et replay distincts conservés. Probe R5B préparé avec gardes FRAM actuelles, nouvelles identités5/12–5/13/run11 et fenêtreHELD20s ; jamais flashé.

**Résultats réellement conservés :**561tests.NET PASS/0skip, dont211Service et35Transport ; sept scénarios JS sous DOMsimulé PASS ; build.NET0warning/0error ; publication win-x64 autonome réussie. ARM canonique et probe PASS ; BINcanonique85b1…c0f5 inchangé. ProbeBIN147640octets SHA2cf491cc8cfa8da76fe97512b3fb6471a753b3ac89ef9b169bf05745075a27ee, ELF4750312506e1ff726240cf180e707d4945e2e43cb28b5a8f62af24677004ccb7.146tests firmwareR5AFIX et sanitizers sur cœur inchangé réutilisés, non répétés. Cross-build limité justifié par modifications harness ; suitePC complète par point d'entrée/transport partagés.

Smoke réel sous Linux sur **chemin série volontairement absent** : HTTP200 sur/,/live.html,/live.js,/api/v1/fleet,/api/v2/live ; POSTcommandes404 ; SQLite54observations exclusivementCommunicationLost/snapshotnull/run-null/epoch-null, integrityok, retour0. **Aucune donnée physique ni test navigateur graphique**. Les tests conservent les vraies captures corrigées positives et les anciennes sévérités240 négatives ; aucune tolérance nouvelle.

Échecs documentés : buildMSBuild parallèleMSB4166 puis-m:1 réussi ; script nonexécutable puis invocationbash réussie ; préparation de garde du modèle hôte erronée78 au lieu70, assertion puis correction réussie. Aucun test supprimé, affaibli ou résultat physique inventé. Tous logs conservés. Modèle FRAM prédit2b501a…7dd84 après commandes futures, **pas un snapshot physique ni une image à restaurer**.

## Matériel et permissions effectivement exercées

Lecture/debugGDB qualifiés seulement ; aucun flash/reset/commandeModbus/writeFRAM/SD/HP/OptionBytes/RDP. Préflight **17/10/2026 11:56:42UTC** : STLINK003500463235510B37333439, UID004A003D/38425018/20333157, M016CommissioningReadOnly, WHO7B/ODRoff/FIFObypass/TSoff. Flash2097152octets SHA `c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26`, FRAM262144octets SHA `4e7fabde5da1100cf9ae8faf262ea079727856826bb2829c1e99b68ad63b84bd`, exactes R5AFIX. Sauvegardes intégrales versionnées compressées ; GDBdétaché. M016 n'ayant jamais été remplacé, aucune restauration supplémentaire nécessaire. Depuis interruption : aucun accès matériel nouveau, ne pas traiter le préflight daté comme télémétrie actuelle.

COM4FTDI0403:6001/BG04KSSWA inventorié, COM3STLINK exclu ; aucun processus ciblé concurrent à11:57:40UTC. Cette inspection ne prouve pas tous les propriétaires possibles de handles. L'unique tentativeWindows utilisaitCOM99 et échoue avant toute ouverture série. **Aucun COM4 ouvert, aucune émissionRS485, aucune admissionR2, run11 et clefs5/12–5/13 non consommés par R5B.** Historique/tail/epoch/reçus inchangés ; aucunrollbackFRAM. L'exclusivitéCOM4 et les budgets physiquesR5B restent non démontrés.

## Reprise précise et critères non satisfaits

Voir [note R5-B](../../Supervision%20TR2/LIVE_R5B.md), procédures de lancement et ordre du contrôle des maîtres. Après résolution autorisée du déploiement Windows : revérifier état/main/identités ; finaliser le contrôleur R5B séparant les maîtres avec fermeture vérifiée ; flash/readback du bon probe ; readonlyB0/B3, véritableSTART admissible, service directRUNNING/perte-reprise, vraiSTOP puisHELD ; corréler bytesCRC/SQLite/API/renduWeb et valeurs distinctes si observées ; mesurer FIFO/budgets puis restaurerM016 et auditerFRAM complète sansrollback. Ne pas lancer l'ancienne recetteR5AFIX5/10–5/11. Les scripts de restauration hérités archivés dans les preuves sont préparatifs **non exécutés**, pas une qualification du contrôleurR5B.

Restent non satisfaits : mesureLIVEphysique par le vrai service surCOM4, corrélation et variation réelles SQLite/API/Web, exclusivité matérielle/connexionperte-reprise en acquisition, HELDphysique reçu par le service, budgetsR5B et audit après nouvelles transactions. Aucun statutDONE/R6/production/industrie. Pas de test déjà validé à refaire sur code inchangé.

## Preuves, préservation et publication

[Preuves R5-B](preuves/TR2-20261017-R5B/README.md) : BUNDLE76artefacts et SHA256SUMS, événementsWindows, logs/TRX, smokeHTTP/SQLite, imagesARM jamaisflashées et préflightGDB complet. Originaux `/home/lolo/dev/msm/reports/TR2-r5b/` ; worktree `/home/lolo/dev/msm/projects/MSM_vibe-R5B-live-to-web`, branche `codex/r5b-live-to-web`.8079fichiers protégés/65autresworktrees/deuxstashes préservés ; dépôt historique main82169af propre. Aucun reset/clean/merge/rebase/forcepush. Les archivesR5-AFIX/R5-A et dettesA04 ne changent pas.

Journal `ETAT_COURANT_TR2.md` et roadmap actualisés factuellement : checkpoint logiciel et blocageWindows, dernier préflight daté, aucune nouvelle qualification, prochaines étapes. Mission et gouvernance inchangées. Autorisationscommit/push/debug exercées ; flash autorisé mais non exercé. Le commit logiciel identifie exactement le code évalué ; les commits de rapport/preuves seront indiqués après publication.

**Publication distante : à vérifier après le push de ce bilan.** StatutBLOCKED pour le refusWindows, non pour cette vérification encore en cours. La relecture distante et le SHA final seront consignés ensuite, sans transformer la mission enDONE.
