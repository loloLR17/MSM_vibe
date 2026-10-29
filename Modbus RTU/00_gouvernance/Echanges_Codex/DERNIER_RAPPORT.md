---
mission_id: TR2-20261028-ACCEL-FS8G-VIBRATION-AC
status: BLOCKED
base_ref: main
base_sha: e7f17f2cd2b8470244a99f7c9e73e9d0f43b3dea
initial_head: e81df0e41207db087485f1e0c49db0fef60af2d0
result_sha: ae6e2d5e9cd2d161abeda1659d99296a0c274ab8
created_at_utc: 2026-10-29T10:26:00Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
revision: AC1-R2-20261029
---

# AC1-R2 — investigation Windows exécutée, moyen de signature externe requis

**BLOCKED, motif précisé par des preuves nouvelles.** La révision de reprise Windows publiée au **778776be6377ad80734aa72f7f2053146a4d70bf** a été lue depuis `origin/main` et exécutée sans redéployer ni redémarrer le service. Les refus correspondent à deux DLL sans signature Authenticode. Leurs packages NuGet d’origine sont cryptographiquement vérifiés ; cela ne signe pas les DLL pour Windows. Aucun certificat de signature de code n’est disponible dans les magasins personnels utilisateur/machine inspectés.

La voie concrète identifiée est un **canal de signature éditeur approuvé**, local ou distant, pour une publication complète et vérifiable. Ce canal n’est pas disponible dans les ressources inspectées ; aucun gate identique n’est répété, aucune protection contournée. Le service démontré reste actif et le TR2 répond STOP. **Les critères ±8g et dynamique AC1 LIVE demeurent non qualifiés** ; aucun objectif n’est transformé en étude hors ligne et aucun lot R1-DSP/ARM n’est démarré.

## Contrat, Git et préservation

- Même `mission_id`, permissions inchangées. `READY` de la révision autorise l’investigation, pas la présomption de déblocage. L’addendum prioritaire du28/10 et l’autorisation expérimentale AC1v1 restent applicables ; B3 absolu et LIVE R0 gelé ne changent pas.
- Ancien worktree de publication propre àe81df0e4 ; dépôt historique principal propre à82169af. Un premier fetch a échoué réseau (RPC/earlyEOF), puis un fetch HTTP/1.1 a effectivement récupéré778776be. Aucune conclusion tirée de la référence locale périmée. Nouvelle branche locale `codex/ac1-r2-windows-trust` basée sur ce HEAD, sans merge/rebase/reset, dans le worktree `MSM_vibe-accel-fs8g-ac-resume`.
- `AGENTS.md`, gouvernance, protocole, journal et précédent rapport vérifiés : aucun changement depuis la relecture précédente, hors révision de mission et documents métier explicitement examinés. Le rapport BLOCKED précédent est conservé comme [snapshot de preuve](preuves/TR2-20261028-ACCEL-FS8G-AC1/windows-trust-resume-20261029/README.md), sans réécriture historique.
- Deux stashes6251b988… et f3b829e6… inchangés ; ancien worktree AC1 et ses deux modifications initiales conservés. Aucun nettoyage ou changement de référence d’archive demandé. La branche de retour pré-SPI2 n’est pas modifiée.

## Opérations réellement effectuées

Lecture des manifestes complets, empreintes des fichiers présents, Authenticode et chaînes des signataires trouvés, flux NTFS exposés, magasins de certificats personnels, provenance NuGet et signatures d’archives. Collecte du journal CodeIntegrity existant, y compris3089 et ActivityID, sans activation d’un journal ni modification de configuration. Lecture de l’API déjà active, inventaire du processus et de deux modules chargés, SELECT SQLite natif Windows en lecture seule et lecture du ledger.

**Aucune modification du firmware, des binaires Windows, des packages NuGet ou du service.** Aucun nouveau build, test fonctionnel, gate COM99, lancement R1/R2, ouverture directe COM4 par Codex, START/STOP/APPLY, reset, GDB, flash, FRAM, microSD, OptionBytes ou politique Windows. Le poller démontré continue ses lectures habituelles indépendamment des seules lectures API effectuées ici. Aucune acquisition n’est déclenchée pour contourner le blocage.

## 1. Paquets exacts et contexte

R1 `TR2-AC1-FS8G-20261028\app` et R2 `TR2-AC1-FS8G-20261028-R2\app` : **378 fichiers chacun**, tous égaux à leurs manifestes de publication antérieurs. Entre les deux,361fichiers identiques et17différents (binaires/PDB des projets et hôte). Les deux DLL refusées sont exactement identiques :

| DLL | SHA-256, R1 = R2 | Authenticode |
|---|---|---|
| `NModbus.Serial.dll` | `1714c12265bf336cff5fd776f97f922fea9b3f0291ccd06e5ccc6c23d41be1c0` | NotSigned |
| `SQLitePCLRaw.batteries_v2.dll` | `815ce410c9eab531dd219318e3ba16c5e38e83d361cfbbdbb4bbca222c497b0b` | NotSigned |

Les14composants ciblés de chaque paquet donnent13NotSigned et1Valid. `Microsoft.Data.Sqlite.dll` est signé par Microsoft ; chaîne inspectée avec révocation désactivée pour ce diagnostic hors ligne, **pas une nouvelle validation de révocation**. Aucun signataire/chaîne de certificat n’existe dans les deux DLL NotSigned. Les fichiers ciblés n’exposent actuellement que `:$DATA`, sans Zone.Identifier observé ; aucune métadonnée retirée/modifiée.

Les deux publications étaient complètes, réalisées par `dotnet publish` puis copie intégrale vers deux répertoires NTFS distincts. Chemin, heure et binaires parents diffèrent. **Aucune cause unique de différence d’acceptation R1/R2 n’est démontrée** : une égalité de hash ne reconstitue pas la réputation cloud ou le contexte historique de chargement. Les gates conservés restent : R1 PASS28/10 ; R2 FAILED29/10, HTTP503/296refus3033-3077/SAC1. Le résultat n’est pas déduit du seul retour0 du processus.

## 2. Corrélation CodeIntegrity 3033 / 3077 / 3089

Le journal est actif, circulaire, contient874records lors du relevé ; records1–874 conservés, derniers événements datés du gate R2. La première requête filtrée temporellement n’a renvoyé aucun événement. Le contrôle de rétention a contredit ce résultat ; une lecture bornée aux2000derniersrecords, sans modification du journal, permet la corrélation correcte. Aucun «3089 absent» ou «logs perdus» n’est conclu de la première requête.

Résultat vérifié : **592 événements pertinents dans148ActivityID**. Chaque groupe possède1ID3033,1ID3077 et2ID3089, soit296refus et296informations de signature.146groupes visentNModbus.Serial,2SQLitePCLRaw.batteries_v2. Exemple dernier groupe `{7a590bd0-5d41-0004-862b-a77a415ddd01}` : records871/873 de refus et872/874 de signature.

Dans **tous les3089 corrélés**, `TotalSignatureCount=0`, publisher/issuer`Unknown`, aucun hash de certificat, dates sentinelles1601. Ces valeurs prouvent l’absence de signature trouvée dans ces observations ; **elles ne prouvent ni certificat révoqué ni certificat expiré**. `VerificationError=0` sans signature ne vaut pas autorisation. Le3077 indique`0xc0e90002`, niveau demandé2/validé1, politique`0283ac0f-fff1-49ae-ada1-8a933130cad6`.

La [documentation Microsoft sur les événements](https://learn.microsoft.com/fr-fr/windows/security/application-security/application-control/app-control-for-business/operations/event-id-explanations) définit la corrélation par ActivityID et les3089 sans signature. Aucun groupeR1 pertinent n’est trouvé dans les records conservés ; son gate réussi reste établi par ses preuves HTTP/SAC antérieures, **pas expliqué** par cette absence d’événements. `CacheState=3` est conservé brut sans lui attribuer une signification non vérifiée. HTTP503 concerne le chargement/stockage PC, pas le firmware ou le RS485.

## 3. Provenance NuGet et signatures distinctes

Source de restore enregistrée : `https://api.nuget.org/v3/index.json`. Versions réellement sélectionnées : NModbus3.0.83, NModbus.Serial3.0.83 ; Microsoft.Data.Sqlite10.0.12 et bundle transitif SQLitePCLRaw.bundle_e_sqlite3 **2.1.12**, pas2.1.11 présumé.

**Commande nouvelle appropriée à ce diagnostic :** `dotnet nuget verify <les trois .nupkg locaux> --all --verbosity normal`, retour0 ; NModbus, NModbus.Serial et SQLitePCLRaw.bundle_e_sqlite3 sont tous **Successfully verified**, signatures de dépôt NuGet.org/Microsoft et horodatage inspectés. Il s’agit d’une vérification de provenance, pas d’une répétition des tests logiciels déjà verts. Les deux DLL déployées correspondent intégralement aux membres `lib/net6.0/NModbus.Serial.dll` et `lib/netstandard2.0/SQLitePCLRaw.batteries_v2.dll` des archives vérifiées ; NModbus.dll net6.0 est également identique.

Le hash de contenu NuGet/metadata est distingué du SHA512 de l’archive signée complète ; ce dernier correspond au fichier`.nupkg.sha512` du cache. Une première comparaison directe de ces deux notions différentes a été refusée par assertion ; aucune corruption n’en a été déduite et la vérification NuGet réelle a tranché sur les archives. Microsoft.Data.Sqlite métapackage n’est pas inclus dans la commande de vérification de ces trois archives ; sa DLL runtime a été inspectée séparément par Authenticode.

Une [signature NuGet](https://learn.microsoft.com/en-us/nuget/reference/signed-packages-reference) garantit l’intégrité/provenance du package, **sans fournir une signature PE Authenticode à chaque DLL extraite**. Réutiliser/télécharger les mêmes packages intègres ne constitue donc pas un changement pertinent permettant de rejouer le gate.

## 4. Chemin conforme et plus petit arbitrage nécessaire

**Aucun certificat codeSigning** dans`Cert:\CurrentUser\My` ou`Cert:\LocalMachine\My` au relevé. Aucune chaîne de signature configurée trouvée dans les projets/scripts inspectés. Cela borne la conclusion aux ressources de cette session : aucun service de signature distant non configuré n’est présumé disponible. Les certificats Microsoft publics examinés ne donnent aucun droit ni clé de signature pour notre publication.

La voie légitime identifiée est de **fournir/désigner un canal de signature éditeur approuvé et utilisable** : certificat codeSigning déjà légitime avec accès autorisé à sa clé, ou profil distant Artifact Signing équivalent déjà approuvé. Alternative à faire fournir par les éditeurs : une distribution reconnue des dépendances, avec mêmes garanties fonctionnelles/provenance. Aucun artefact alternatif Authenticode n’est disponible dans les paquets/cache inspectés ; pas de remplacement ou mise à niveau spéculative.

Le livrable de ce canal doit être un **paquet complet vérifiable**, composants concernés signés et horodatés, provenances/licences préservées, sans mélange avec R1/R2. L’[orientation Microsoft pour la signature SAC](https://learn.microsoft.com/en-us/windows/apps/develop/smart-app-control/code-signing-for-smart-app-control) est référencée dans la [note opérateur](../../Supervision%20TR2/operator-demo/AC1_WINDOWS_TRUST.md). Aucune clé privée ne doit être mise dans GitHub ; aucun achat, auto-certificat, import de racine, modification de stratégie, suppression ADS/EA ou changement d’empreinte destiné à faire passer le refus n’a été effectué.

La [FAQ Microsoft](https://support.microsoft.com/fr-fr/windows/security/threat-malware-protection/smart-app-control-frequently-asked-questions) ne propose pas d’exception individuelle SAC. Certificat valide ou nouvelle signature ne sont pas une promesse de réussite : **après changement pertinent attesté seulement**, gate complet sur COM99,4HTTP200/SACactif/zéroCIcorrélé/SHAs, puis qualification physique si la cible est sûre. L’absence de cette ressource est le blocage externe concret, sans besoin de décision sur le firmware à ce stade.

## 5. État final, acquis conservés et prochaine action

- Service démontré **PID8312**, même processus et ancien paquet `TR2-LIVE-COM4-HELD-20261019`, sans redémarrage. Les deux modules concernés sont effectivement chargés depuis ce paquet historique ; cela ne prédit pas le résultat d’un nouveau chargement de R1/R2.
- À **10:25:28UTC selon le relevé Windows**, API réelle HTTP200, transport connecté, B3STOP/flags4097/seq0 sans mesure valide ; SQLite **133758observations**, lecture native `readOnly:true`, SELECT uniquement ; ledger **Device1/Epoch5/Transaction25** inchangé. Ces observations sont datées et ne remplacent pas un audit physique de mémoire.
- STM32 : image e1846c…aa7b80 installée au lot précédent, aucune opération de cible ici ; pas de nouvelle lecture Flash/FRAM prétendue. Configuration8g non appliquée ; dynamique AC1 non qualifiée. Les preuves du premier boot perturbé par GDB et du reset de récupération précédent restent inchangées. SD, FRAM, identités, journaux, OptionBytes, lanceurs et sauvegardes préservés ; aucun rollback/restaurationM016.
- Tests acquis **148C/ARM, Protocol52/Service253 et autres suites**, builds et preuves précédentes réutilisés. Pas de compilation/tests fonctionnels nouveaux requis : uniquement documentation et collecte readonly, aucune modification du code applicatif/firmware. `result_sha` reste celui du correctif PC effectivement évalué, ae6e2d5e ; image physique issue de72a7b573, non confondue avec le SHA documentaire.
- Après fourniture du canal conforme : signer/publier une version complète séparée, gateCOM99, état/ledger frais et handoff contrôlé, baselineAC1±2g, vraiAPPLY8g/B4/registreCTRL1_XL, persistance et START/STOP, preuve dynamique séparée B3 et chaîne SQLite/API/Edge/CSV, budgets et audit sûr. Ne jamais halter pendant la calibration de boot. Aucun essai machine±6g inventé, aucune qualification indépendante ou DONE revendiqué.

Journal `ETAT_COURANT_TR2.md`, note opérateur `AC1_WINDOWS_TRUST.md` et document `REPRISE.md` actualisés. Gouvernance et contrat de mission inchangés par Codex. [Preuves structurées et commandes](preuves/TR2-20261028-ACCEL-FS8G-AC1/windows-trust-resume-20261029/README.md). Preuves locales : `/home/lolo/dev/msm/reports/TR2-accel-fs8g-ac/windows-trust-resume/`.

Publication normale surmain et relecture intégrale distante à consigner après push. Le statutBLOCKED désigne la ressource externe manquante, pas une interruption de quota. La mission conserve son identifiant et son objectif ; R1-DSP/ARM reste en attente.


## Publication et relecture distante

Le bilan **a13e29d5c0580326eec939cb63a0ef3761406394** est publié par push normal surmain. À **10:30:55UTC le29/10**, référence effectivement récupérée de GitHub : **24fichiers relus intégralement et byte-identiques**, **18artefacts décompressés** avec vérification de leurs empreintes brutes et stockées. [Trace de relecture](preuves/TR2-20261028-ACCEL-FS8G-AC1/windows-trust-resume-20261029/remote-readback.json). StatutBLOCKED maintenu : cette publication vérifie les preuves, pas le déblocage Windows ou la qualification physique.

Worktree de publication propre/HEAD égal àmain distant au bilan ; ancienmain propre82169af, ancien worktree avec ses deux modifications préexistantes et deux stashes conservés. Le SHA de la mise à jour documentaire enregistrant cette relecture est fourni dans la réponse finale après vérification distante. Aucun changement matériel, service ou données pendant la publication.
