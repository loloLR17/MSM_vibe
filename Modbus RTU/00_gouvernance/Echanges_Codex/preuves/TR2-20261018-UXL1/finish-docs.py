from pathlib import Path
import subprocess,json,datetime
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-UXL1-live-readonly');p=Path('/home/lolo/dev/msm/reports/TR2-uxl1');code=subprocess.check_output(['git','rev-parse','HEAD'],cwd=r,text=True).strip();initial=json.loads((p/'initial-state.json').read_text())['initial_head'];now=datetime.datetime.now(datetime.timezone.utc).isoformat()
note=f'''# UX-L1 — parc et fiche LIVE en lecture seule

Mission `TR2-20261018-UXL1-LIVE-READONLY-INTEGRATION`, code `{code}`, HEAD initial `{initial}`. Aucun accès COM4, commande Modbus, flash ou debug. La conception UX publiée et les interfaces V1/S7 restent inchangées.

## Pages et source d'autorité

- `/` conserve sa Vue générale V1 et ajoute une section distincte Parc LIVE.
- `/live-fleet.html` affiche une ligne par identité TR2 connue dans la couverture, avec endpoint explicite en l'absence d'affectation équipement/local raccordée.
- `/live-device.html?deviceId=1` conserve six onglets : Synthèse, Vibrations, État & configuration, Commandes, Campagnes, Diagnostic. Les deux premiers affichent global et axes ; les autres décrivent les informations disponibles et les limites. Aucun bouton de commande ni script B5 n'est chargé dans cette fiche readonly.
- `/live.html` et `/api/v2/live` restent le diagnostic R5 existant ; `/api/v1/fleet`, les fiches V1 et commandes S7 conservent leurs contrats.

La nouvelle API additive `GET /api/v2/live/overview` combine les vues du service LIVE existant et les dernières1000 observations SQLite `live_observation`. Pas de migration, modification de rétention ou second moteur Modbus. Les données ne passent pas par les DTO V1. L'API déclare sa couverture bornée : elle ne prétend ni inventaire exhaustif ni historique illimité. Une longue perte peut sortir une ancienne mesure de cette couverture ; elle ne sera pas inventée.

La projection est sans cache mutable. Une identité durable connue déduplique les endpoints ; un endpoint sans identité reste explicitement non identifié. Une identité différente ne reprend pas la mesure de l'ancien capteur. Sur perte d'identité, `lastKnown` conserve son propre propriétaire et sa date ; il ne devient jamais l'identité courante. Une erreur d'archive est exposée séparément, et `ArchiveState` du service est conservé.

L'UI montre RMS/crête en mg, sévérité transmise brute et interprétée, communication, V/F/HELD, réception PC et ancienneté. Les valeurs retenues sont marquées périmées, avec propriétaire et date. Aucune absence remplacée par zéro. L'état d'acquisition est limité à ce que déclare la dernière observation B3, pas extrapolé à l'instant présent. UTC fenêtre, run/epoch/config non exposés restent inconnus. `LiveDiagnostic` est marqué NON-PRODUCTION. HTML rendu par textContent, aucune écriture HTTP, aucun cache métier navigateur.

Les archives et le replay ont toujours `historical=true` et `isCurrent=false` dans la projection opérationnelle, même si le B3 archivé déclarait FRESH. Le chemin direct existant peut utiliser sa fraîcheur actuelle sans nouveau polling Modbus ; il n'a pas été lancé durant cette mission. Le diagnostic R5 garde son contrat historique distinct.

## Consultation reproductible sans matériel

Depuis `Modbus RTU/Supervision TR2`, sur une **copie d'archive fermée, checkpointée et figée**, sans writer actif :

```bash
dotnet run --project src/TR2.Supervision.Service --no-build -- \\
  --live-archive /chemin/externe/archive-figee.db --listen http://127.0.0.1:5093
```

Ouvrir `/live-fleet.html` ou `/live-device.html?deviceId=1&view=vibration`. Le même service S7 est utilisé, sans usine série, polling de replay ou command sink. Le mode figé refuse un WAL non vide. Le lecteur est en ReadOnly et utilise `immutable=1` exclusivement pour une archive déclarée figée ; ne pas utiliser ce mode sur une base active. La lecture d'une base vivante dans les compositions direct/replay reste en ReadOnly normal, compatible avec leur WAL. Aucun changement de politique SQLite existante. Référence : [URI SQLite et immutable](https://www.sqlite.org/uri.html).

Le smoke a décompressé une copie de l'archive R5-B2 publiée.528 observations réelles historiques, SHA `6efaa77972d41cd749bf16e38e23aa22911d889fe5d9453504c80435cf0f5fd1`. La base reste byte-identique, aucun WAL créé. La preuve DOM affiche notamment HELD1073/1157mg global, axes RMS77/1068/68mg, crêtes134/1151/126mg, sévérité0, UTC indisponible et historique explicite. Ce n'est pas une acquisition actuelle ni un nouveau test physique.

## Validation et limites

Build .NET sans avertissement ni erreur. Première suite complète :350 tests hors Service réussis ;214 Service réussis et un échec nouveau sur la création WAL en lecture seule ordinaire. Cette assertion a été conservée : ajout du chemin SQLite figé adapté, refus explicite d'un WAL non vide, test de conservation du fichier et des valeurs. Ensuite215 Service passent, puis217 après ajout du contre-test WAL et de l'intégration HTTP. Les six nouveaux tests couvrent capture/archive R5-B2, identité et perte, replay non courant, déduplication, erreurs, readonly et pages/six onglets/POST404. Les tests S7/V1 préexistants passent. Tests JS ciblés sur V/F historique, XYZ, HELD, perte et retenue, absence, UTC et absence d'écriture/cache. Build final des assets et smoke HTTP200, API V1 distincte, POST commandes404. Deux DOM Edge headless réels (parc et Vibrations), pas de navigateur humain ni screenshot.

L'historique persisté existe, mais tendance, multi-axes graphiques, sélection temporelle, filtres/tri avancés et CSV sont différés à UX-L2 pour garder L1 centré sur les données structurées et leurs qualités. Aucun contrôle factice. Les affectations équipement/local ne sont pas inventées. Seuils, overlays d'événements et finitions relèvent d'UX-L3, seulement avec autorités documentées. Aucun score santé, FFT, mm/s, norme ou profil produit nouveau. Les six onglets sont contextualisés readonly ; les interfaces V1 de commandes ne sont pas copiées vers le diagnostic.

Preuves : [UX-L1](../00_gouvernance/Echanges_Codex/preuves/TR2-20261018-UXL1/README.md). Matériel non touché et non relu ; dernier état matériel connu reste celui de R5-B2, M016 restauré/FRAM2b501a…7dd84. R6 et qualification indépendante ChatGPT restent distincts.
'''
(r/'Modbus RTU/Supervision TR2/LIVE_UXL1.md').write_text(note)
report=f'''---
mission_id: TR2-20261018-UXL1-LIVE-READONLY-INTEGRATION
status: IN_PROGRESS
base_ref: main
base_sha: 9c2b65400cbe11564875a94864ab22a896cd3733
initial_head: {initial}
result_sha: {code}
created_at_utc: {now}
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# UX-L1 — parc et fiche LIVE raccordés aux observations persistées

**Tranche logicielle réalisée et vérifiée ; IN_PROGRESS uniquement pour publication/relecture distante restantes.** Aucun blocage technique. Le parc LIVE et une fiche Synthèse/Vibrations readonly sont intégrés au host S7, avec distinction explicite entre historique et réception physique actuelle. Pas d'acquisition nouvelle, COM4 non ouvert, aucun flash/debug/commande STM32.

Code évalué **`{code}`**, `feat(uxl1): integrate readonly LIVE fleet and sensor views with persisted provenance`.13 fichiers PC/Web/tests,408 lignes ajoutées/5 retirées ; aucune modification du firmware, gouvernance, norme ou archives R5-B2.

## Livré

- Section Parc LIVE dans `/`, page `/live-fleet.html`, fiche `/live-device.html?deviceId=1` à six onglets, données structurées Synthèse/Vibrations et limites honnêtes dans les autres vues.
- API additive `/api/v2/live/overview`, projection sans cache autonome et lecteur SQLite borné aux1000 observations récentes ; ni migration ni rétention changées. Identité durable connue dédupliquée, endpoints inconnus explicites, aucune affectation équipement/local inventée.
- RMS/crête globales et X/Y/Z en mg, sévérité B3 brute/interprétée, communication et qualité distinctes, réception PC et ancienneté, état acquisition déclaré dans l'observation. HELD, repeated sequence, perte/snapshotnull, `IsCurrent:false`, `ArchiveState` et erreur d'archive conservés. Dernière mesure retenue étiquetée périmée, avec son propriétaire et sa propre date ; jamais promue en identité ou mesure actuelle.
- Nouveau lancement du **même service** `--live-archive <copie-figee.db> --listen http://127.0.0.1:5093`, sans transport/polling Modbus, writer ou replay. Historique/replay toujours non courant dans la nouvelle projection. Compositions LIVE existantes raccordées additivement, sans nouveau maître ; elles n'ont pas été exécutées sur matériel ici.
- `/live.html` et `/api/v2/live` conservés, pages/API V1 et commandes S7 préservées. Les nouvelles fiches n'exposent aucune action B5/POST. `LiveDiagnostic` marqué NON-PRODUCTION ; B1/B2/B7, UTC fenêtre et identités run/epoch/config non exposés restent inconnus.

[Note d'implémentation et lancement](../../Supervision%20TR2/LIVE_UXL1.md), [preuves](preuves/TR2-20261018-UXL1/README.md). Les graphiques/tendances et filtres avancés restent UX-L2 : l'historique existe mais L1 privilégie les données structurées et la qualité, sans courbe interpolée ou bouton factice. Aucun accès distant, auth, export, seuil, score, FFT ou mm/s ajouté ; UX-L3/R6 distincts.

## Validations réellement exécutées

Build .NET réussi sans avertissement/erreur. Première suite complète : **350 tests hors Service PASS**, **214 Service PASS et1 échec nouveau**. SQLite ReadOnly ordinaire créait du WAL/SHM sur la copie : assertion conservée, chemin d'archive figée corrigé avec URIimmutable, garde refusant un WAL non vide ; aucune relaxation du test ni masquage. Ensuite215 Service PASS ; après ajout de deux scénarios de garde/HTTP, **217 Service PASS/0skip**. Les anciens tests V1/S7/B5 sont inclus. Ne pas présenter cette séquence comme une nouvelle suite complète de567 tests exécutée après la dernière modification.

Les six nouveaux tests couvrent R5-B2 réel relu et étiqueté historique, V/F, HELD, perte et identité inconnue, changement d'identité sans fuite d'ancienne mesure, replay non courant, déduplication, erreurs, conservation des octets, refus WAL et API/pages/six onglets/absence de commandes. Tests JS ciblés sur captures R5-B2 réelles, XYZ, sévérité, UTC absente, perte/retenue/absence sans zéro, absence d'écriture/cache/innerHTML. Build final des assets et smoke HTTP avec le service Linux réel :200 sur/, parc, fiche, diagnostic, nouvelle API etAPI V1 ; POST commandes404.

**Deux DOM Edge headless réellement rendus**, parc et onglet Vibrations, depuis une copie de l'archive physique R5-B2. Historique/LiveDiagnostic/NON-PRODUCTION/mg visibles ; HELD et dernière valeur périmée affichés, UTC absente.528 observations historiques dans la copie, base byte-identique SHA6efaa77972d41cd749bf16e38e23aa22911d889fe5d9453504c80435cf0f5fd1, aucun WAL créé. Aucun navigateur humain, screenshot ou mesure physique nouvelle revendiqué.

Le mode d'archive figée nécessite une copie stable, checkpointée et sans writer ; il refuse un WAL non vide. Les lectures des bases vivantes utilisent le ReadOnly normal, sans immutable. La limite1000 est explicitement affichée : pas d'inventaire exhaustif, une valeur hors couverture n'est pas inventée. Voir la note pour les limites de rafraîchissement et les URI SQLite documentées.

## Préservation, état et publication

Autorisations exercées : logiciel, compilation/tests, commit/push ; flash/debug interdits et non utilisés. Aucun accès au STM32, FRAM, Flash ou COM4, pas de campagne/build firmware répété. État matériel R5-B2 hérité, **non relu** dans cette mission. Journal et roadmap actualisés uniquement pour cette capacité logicielle et ses limites ; gouvernance UX/S7 et archives immuables préservées.

Dépôt historique main82169af propre, autres worktrees et deux stashes préservés. Worktree `MSM_vibe-UXL1-live-readonly`, branche `codex/uxl1-live-readonly`. Les sources courantes PC explicitement modifiées dans le périmètre sont distinguées des archives/proofs protégés. Aucun reset/clean/merge/rebase/force push.

Publication du bilan puis relecture intégrale depuis GitHub encore à terminer ; le statut IN_PROGRESS concerne uniquement cette étape. Le SHA relu sera consigné avant DONE. Contrôle indépendant ChatGPT distinct de la livraison logicielle.
'''
(r/'Modbus RTU/00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md').write_text(report)
j=r/'Modbus RTU/00_gouvernance/ETAT_COURANT_TR2.md';s=j.read_text();s=s.replace('# État courant MSM TR2 — synthèse opérationnelle','# État courant MSM TR2 — synthèse opérationnelle\n\n**Actualisation logicielle UX-L1 :** parc et fiche LIVE readonly raccordés à SQLite/API, données historiques explicitement non courantes. Aucun accès matériel ; dernier état cible connu reste celui de R5-B2 ci-dessous, non relu.',1);s+=f'''\n\n## UX-L1 —18/10/2026, intégration Web readonly\n\n- **VALIDÉ LOGICIEL** : code `{code}`, parc LIVE intégré à la Vue générale et fiche à six onglets, Synthèse/Vibrations avec RMS/crêtes/axes en mg, sévérité transmise, provenance/qualité et dernière valeur périmée. API additive overview et SQLite borné1000, sans cache métier ou nouveau polling Modbus ; V1/S7 et diagnostic R5 conservés. Mode archive figée sans série, historique/replay jamais courants.\n- **PREUVES** :350 tests hors Service puis217 Service PASS, JS sur captures réelles, HTTP/API local et deux DOM Edge headless depuis archive R5-B2. Copie528 observations byte-identique, aucun WAL ; aucun essai physique nouveau. Échec initial WAL conservé/corrigé par mode figé adapté, garde et assertion maintenues.\n- **LIMITES / SUITE** : tendance/graphiques/filtres/CSV UX-L2, affectations équipement/local non raccordées, seuils/overlays UX-L3. Pas de qualification produit/R6 ; aucune commande. Archives/proofs R5-B2 préservés ; matériel M016/FRAM2b501a hérité non relu. [Note UX-L1](../Supervision%20TR2/LIVE_UXL1.md), publication/relecture puis contrôle indépendant ChatGPT.\n''';j.write_text(s)
road=r/'Modbus RTU/00_gouvernance/ROADMAP_TR2_VERS_SUPERVISION_VIBRATOIRE.md';road.write_text(road.read_text()+'''\n\n## Constat UX-L1 —18/10/2026, intégration logicielle readonly\n\nParc et fiche LIVE à six onglets raccordés au modèle R5 et à SQLite, avec API additive, provenance et historique non courant explicites. Tests .NET/JS/HTTP et DOM headless sur archive R5-B2 ; aucun accès matériel ou qualification supplémentaire J4/J5/R6. Tendance, graphiques, filtres avancés et export différés UX-L2 ; overlays documentés UX-L3. [Note UX-L1](../Supervision%20TR2/LIVE_UXL1.md). Gouvernance et acquis physiques inchangés.\n''')
print(code,initial)
