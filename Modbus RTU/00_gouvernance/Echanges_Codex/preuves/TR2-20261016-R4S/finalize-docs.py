from pathlib import Path
import datetime,json
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4S-publication'); p=Path('/home/lolo/dev/msm/reports/TR2-r4s'); g=r/'Modbus RTU/00_gouvernance'; e=g/'Echanges_Codex'
now=datetime.datetime.now(datetime.timezone.utc).isoformat()
report=f'''---
mission_id: TR2-20261016-R4S-SERVICE-BUDGET-SAFE-RECOVERY
status: BLOCKED
base_ref: main
base_sha: a57cc5bcc84e593a4a99f4100600346b6d990eb4
initial_head: 20c8089514b40a75ed59d7c6616b75838482e0c8
result_sha: c2395d9585c95694b628141a2e20b5767807cb7b
created_at_utc: {now}
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R4-S — service diagnostique acquis ; reprise R2 bloquée sur la retraite du journal

**BLOCKED en phase 2, après phase 1 PASS.** Le traitement R4-M/R4-P remet à zéro
la génération du journal, alors que le contrat exige « aucun reset des compteurs ».
Ce constat est reproduit sur copie RAM authentique, sans écriture sur la cible.
Une décision sur ce compteur est nécessaire avant toute maintenance du STARTED5.
La phase 3 conditionnelle n'a pas été lancée. **M016 est restauré, Flash et FRAM
intégralement identiques à l'état initial ; aucune nouvelle admission R2.**

## Tranches et mesures physiques

Préflight : TR2-001, NUCLEO-U575ZI-Q, ST-LINK `003500463235510B37333439`,
UID `004A003D/38425018/20333157`, sauvegardes authentiques et récupérables.
Les archives R4-R déjà présentes correspondent aux fichiers au commit `7ebd712` ;
aucune duplication ni réécriture. Lecture du contrat, journal, roadmap, norme R0,
R3-B/R4-M/D/P/Q/R et procédures qualifiées avant intervention.

Trois tentatives distinctes conservées ; seuils work/service/gap inchangés :

| Essai volatil readonly | Résultat | Mesures maximales work / service / gap |
|---|---|---|
| Image 01, affine fixe | Arrêt après 1,206563 s : résidu horloge 109 > 100 µs | 335 / 2322 / 2457 µs |
| Image 02, suivi + profilage | Arrêt après 746 XYZ : service 2511 > 2500 µs | 335 / 2511 / 2493 µs |
| Image 03, feuilles statistiques en O2 | **PASS borné, 10,001045 s** | **333 / 1804 / 1902 µs** |

Essai 01 : 31787 XYZ / 3974 TS ; première fenêtre diagnostique N=4096 conforme
à l'oracle brut indépendant (RMS global/X/Y/Z 960/953/102/40 mg ; crêtes
997/989/135/79 mg). Ce préfixe n'est pas un PASS dix secondes.

Essai 02 : la paire de service maximale vaut 2507 µs, dont poll 1991 µs et RTU
513 µs. Le poll inclut le consommateur 789 µs, lui-même incluant métrologie
556 µs et datation 102 µs. Les feuilles `window_statistics` et
`indicator_calculator` restaient compilées en O0 malgré leur appelant O2.
Aucune fenêtre complète. Après STOP, un dump GDB de plage vide (zéro contrôle
horloge) a échoué ; la collecte FRAM terminale a été complétée séparément,
sans relancer l'essai. Les erreurs et sorties originales sont conservées.

Essai 03, code `c2395d9585c95694b628141a2e20b5767807cb7b` :

- 265669 XYZ / 33209 TS reçus ; 265664 XYZ émis et 5 en attente d'ancre.
- Cinq fenêtres diagnostiques et 808 réponses RTU RAM cohérentes/CRC vérifiées.
  Autorité métrologique forcée false, aucune réponse valide autoritative.
- FIFO max 58 ; erreurs FIFO et STOP nulles. START volatil 911 µs, STOP nominal
  232 µs, arrêt de sécurité 233 µs. Aucune commande START R2.
- Marges observées : work 667 µs, service 696 µs, gap 3098 µs sous leurs bornes
  respectives 1000/2500/5000 µs. Il s'agit de maxima observés, pas de WCET garanti.
- Paire maximale 1801 µs : horloge 30, poll 1248 (consommation 351, dont datation
  64 et métrologie 201), RTU 513, publication 0. Ces coûts imbriqués ne se somment
  pas à leurs parents ; les maxima indépendants ne décrivent pas un même appel.
- 99 contrôles de conversion compteur capteur/DWT, résidu maximal 10 µs,
  seuil de refus inchangé à 100 µs.
- Premier N=4096 brut : oracle PASS, RMS global/X/Y/Z **959/953/102/39 mg**,
  crêtes **998/992/138/82 mg**. Aucun étalon vibratoire externe utilisé.
- FRAM entière identique `14fb…2aa9`, zéro tentative/commande/octet d'écriture.

Cette charge compose FIFO réel, datation, calcul, lectures FRAM readonly par
quanta de 32 octets et RTU loopback RAM. Elle ne contient pas les écritures R2
de la phase 3 ; les marges ne prouvent donc pas cette coexistence complète.

## Modifications et validations logicielles

Correction minimale du provider `fifo_event_time.[ch]` : suivi optionnel à partir
des brackets compteur/DWT observées, contrôle du même seuil **avant** correction,
pivot continu au dernier timestamp FIFO ; aucun événement déjà émis n'est redaté,
aucune grille ODR ni UTC inventée. Test +80 ppm : affine fixe refusé, suivi dix
secondes monotone et causal ; les refus d'écart restent couverts.

Probe dédié `stm32_r4s_bench.c`, profil CMake explicite NON-PRODUCTION et garde SHA
FRAM entière ; port média refusant toute écriture. Partage contrôlé des helpers
R4-R via macro d'entrée, sans lancer son ancienne acquisition. Instrumentation
DWT du provider/consommateur activée seulement pour R4-S ; O2 limité aux chemins
chauds du profil, dont les deux feuilles de calcul identifiées, sans changement
d'algorithme. V1, moteur R2 et contrôles de maintenance existants inchangés.

Validation complète justifiée par le changement du provider partagé :
`tr2_validate.sh` avec SDK Linux STM32CubeU5 v1.9.0 : **141 tests hôte PASS
(60,50 s), cross-build canonique PASS** ; gardes de profil et ARM du probe PASS.
Provider et métrologie en O2/ASan/UBSan PASS, dont 448 oracles indépendants.
BIN V1 canonique inchangé :
`85b1be6044d76349a715ea0627b442bd3301e63daa606399b3d83bd5b9bbc0f5`.
Les commandes, compilateurs et sorties effectives sont conservés dans les preuves.
Aucun test réussi sur code inchangé n'a été répété pour la seule publication.

Images physiques 01/02/03 BIN **et ELF** conservées sans perte en zlib avec leurs
profils/toolchain. Image 03 : BIN 126864 octets,
SHA `81ac4a44bb6422f539512f562e6c7e5a241d60b933943eeced84dd0ac729bb6d` ;
ELF SHA `246dfa820116a0fe23849877a1e5d392751a3dbffc600af8cffbc3c8f800b46b`.
Voir `phase1*-build.json`, logs ARM et manifeste SHA256.

## Phase 2 — contre-exemple et décision structurante

Le snapshot authentique contient STARTED epoch 4 / tx 5 / run 5, INTENT conservé.
Les préconditions exactes actuelles `r4m_capture` et `r4p_capture` le **refusent** ;
elles n'ont pas été élargies sur main. Leur mécanisme d'isolation vide les
28672 octets du journal dans une nouvelle image transactionnelle.
`live_command_store_recover` initialise ensuite `generation=0`, puis prend le
maximum des entrées présentes. Reproduction hors matériel sur cette copie :

| Compteur | Avant | Pont isolé simulé | Après vrai RENEW moteur, simulé |
|---|---:|---:|---:|
| Génération du journal `LiveCommandStore.generation` | 26 | **0** | **4** |
| Plancher run | 5 | 5 | 5 |
| Epoch | 4 | 4 | 5 |

La génération du média/root reste croissante ; ne pas confondre ces compteurs.
Ce n'est ni un reset du run, ni une preuve de violation explicite de R0 pour
ce compteur interne : c'est l'applicabilité de l'exigence contractuelle générale
« aucun reset des compteurs » qui reste à trancher. La régression 26→0→4 est
factuelle ; l'autoriser ou introduire une nouvelle politique de préservation
requiert la décision structurante prévue en phase 2 et par AGENTS.md §8.

`counterexample_model.c` et `reproduce-counterexample.py` reproduisent ce fait
avec le cœur et la maintenance **inchangés de main**, sur transport RAM. Le modèle
applique volontairement le mécanisme générique au-delà des anciennes gardes de
capture, uniquement pour tester son effet, **sans en déduire une autorisation**.
Retour modèle 7 attendu, diagnostic de reproduction PASS ; critère de
préservation du compteur FAILED (`counter-contract.json`). Aucun outil matériel.

Une étude plus complète a aussi passé mécaniquement 142 budgets de coupure et
quatre phases de RENEW, anti-rejeu des cinq anciennes clés et START run6/STOP
simulés. Sa suite locale de 142 tests PASS ne rend **pas** ce plan conforme au
contrat et ne remplace pas les 141 tests de main. Elle reste isolée au commit
local `86d0f4b2b47ee8160bb459d4c19a2a78c7f4d846`, branche
`codex/r4s-retirement-study`, worktree `MSM_vibe-R4S-service-budget-safe-recovery`.
L'image expérimentale 04 a seulement été compilée : **jamais flashée ni publiée**.
Les logs d'étude sont conservés comme tels, sans intégrer sa maintenance sur main.

**Décision attendue de ChatGPT/utilisateur :** préciser si la génération du journal
doit être préservée dans la retraite, ou si un changement explicite de namespace
avec remise à zéro de ce seul compteur est admissible. Publier ensuite une
procédure/mission révisée avec préconditions exactes du snapshot, protections du
pont, coupures et anti-rejeu. Ne pas remettre à zéro silencieusement, inventer un
COMPLETED du STARTED5, réutiliser run5/clé4-5 ou lancer un firmware LIVE sur le pont.

## État final matériel et limites

Restauration qualifiée achevée **2026-10-16T21:45:31.572741Z** :

- Flash complète **2097152 octets** identique à l'initiale, SHA
  `c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26`.
- FRAM complète **262144 octets** identique à l'initiale, SHA
  `14fb172f2917d0a8e440438051831c84f3f3e746161702a7de9e9a1a251b2aa9`.
  Historique [0,122880) et tail [245760,262144) exacts, aucun rollback.
- Flash standard de 16 pages/131072 octets, relecture intégrale ; **M016
  CommissioningReadOnly WAIT**, capteur arrêté, GDB détaché.
- Zéro écriture FRAM et zéro admission R2 sur tous les essais ; aucun traitement
  physique des phases 2/3, aucune nouvelle identité consommée. STARTED5 conservé.

Aucun mass erase, Option Bytes/RDP, SD, HP/COM/RS-485, changement de câblage ni
intervention physique. Pas de B3 LIVE valide autoritatif, de qualification des
états FRESH/HELD/INVALID en phase 3, de N supérieurs, d'endurance, rollover réel,
dérive SI/thermique ou liaison électrique. **R4 global, R5/R6 et les jalons
historiques non qualifiés restent ouverts.**

## Publication, préservation et reprise

Checkpoint physique `7eb49cd891edadfe7af937b33b06d08bddbfc464` publié et relu
intégralement depuis GitHub ; le manifeste des preuves a été vérifié.
Le présent rapport et les preuves finales sont publiés depuis le worktree
`MSM_vibe-R4S-publication`, branche locale `codex/r4s-publication`, par push normal
explicite `HEAD:refs/heads/main`. La preuve de relecture finale et le SHA du commit
contenant ce rapport sont fournis après publication, sans autoréférence fictive.

Dépôt principal préservé à `82169af7e76797731358852bec83bfb0ceb54ccf`, deux stashes
historiques inchangés (`6251b988…`, `f3b829e6…`), autres worktrees et fichiers
protégés contrôlés dans `preservation-final.json`. Aucun merge/rebase/reset/clean
ni force push. L'étude non publiable reste récupérable dans son commit local.
Journal actualisé (résultat, état matériel courant, décision attendue), roadmap
actualisée factuellement ; gouvernance, normes gelées, dettes A-04 et archives
antérieures préservées.

Preuves : [index et manifeste](preuves/TR2-20261016-R4S/README.md),
[service/oracle](preuves/TR2-20261016-R4S/phase1c-summary.json),
[contre-exemple](preuves/TR2-20261016-R4S/counter-contract.json),
[restauration](preuves/TR2-20261016-R4S/hardware/restoration-summary.json).
Sauvegardes Flash complètes conservées hors Git dans
`/home/lolo/dev/msm/reports/TR2-r4s/hardware/`. Snapshots FRAM compressés publiés.
Reprise : aucun nouvel essai de phase 1 sur code inchangé ; résoudre la décision
ci-dessus, qualifier la maintenance sur copie, puis seulement phases 2/3 sur
cible après nouveau préflight. Contrôle indépendant ChatGPT restant à réaliser.
'''
(e/'DERNIER_RAPPORT.md').write_text(report)
f=r/'Modbus RTU/05_Firmware/LIVE_R4S.md'; t=f.read_text(); t=t.replace('Restauration Flash M016 complète et FRAM inchangée à vérifier après ce probe.','Restauration finale vérifiée ci-dessous ; ces paragraphes retracent les étapes intermédiaires.').replace("Phase2 : audit/coupures/anti-rejeu sur snapshot authentique avant tout traitement\nR2 physique. Le capteur est arrêté ; poursuite entreprobes isolés fermés avec\nbackupM016 vérifié, restauration complète àla fin, sans restauration intermédiaire\ninutile avant chaque flash de probe. Aucune entréeLIVE générique sur un pont.",'''## Résultat final — phase 2 BLOCKED, cible restaurée

Le mécanisme R4-M/R4-P appliqué sur copie RAM authentique fait passer
`LiveCommandStore.generation` de 26 à 0, puis à 4 après RENEW simulé ; runfloor5
préservé et epoch4→5. Les anciennes gardes exactes refusent le snapshot14fb.
Aucune garde élargie ni maintenance n'a été intégrée sur main. Ce compteur est
celui du journal, distinct des générations root/configuration et des identités
run/epoch. L'exigence « aucun reset des compteurs » nécessite une décision sur
sa portée ou une politique explicite de préservation ; aucune exception déduite
de R0. Contre-exemple reproductible avec le code inchangé de main dans les preuves.

L'étude de retraite reste dans le commit local86d0f4b, branche
codex/r4s-retirement-study ; image04 jamais flashée ni publiée. Les coupures et
anti-rejeu mécaniques réussis ne valent pas autorisation contractuelle.
Aucune phase2/3 physique, aucun START/RENEW R2, zéro écriture FRAM. B3
numérique diagnostique ne vaut toujours pas B3 LIVE autoritatif.

Restauration vérifiée le16/10/2026 à21:45:31UTC : Flash2Mio exacte c5bc344d…f5b26,
FRAM262144 exacte14fb172f…2aa9, historique/tail inchangés ; M016
CommissioningReadOnly WAIT, capteur arrêté, GDB détaché. Reprise après décision
structurante, sans rejouer les trois acquisitions acquises sur code inchangé.
Rapport et preuves : ../00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md.'''); f.write_text(t)
f=g/'ETAT_COURANT_TR2.md';t=f.read_text();a=t.index('**Dernière actualisation :**');b=t.index('\n\n> Ce document',a);old=t[a:b];second=old[old.index('**État du second banc'):];t=t[:a]+'''**Dernière actualisation :** 16 octobre 2026, R4-S : phase 1 physique diagnostique dix secondes PASS (work333/service1804/gap1902µs), mais reprise R2 BLOCKED sur la portée de l'interdiction de remise à zéro du compteur de journal. Aucune phase2/3 physique, aucune écriture FRAM. M016 restauré, Flash2Mio exacte c5bc344d…f5b26 ; FRAM14fb172f…2aa9/STARTED epoch4-tx5-run5 inchangée. '''+second+t[b:];a=t.index('- **Cible principale vérifiée');b=t.index('\n- **Code A04',a);t=t[:a]+'''- **Cible principale vérifiée le16/10/2026 (R4-S) :** TR2-001 / NUCLEO-U575ZI-Q, SN003500463235510B37333439, UID004A003D/38425018/20333157. À21:45:31UTC, firmware M016 CommissioningReadOnly WAIT restauré, Flash2Mio exacte SHA c5bc344d…f5b26 ; capteur arrêté/GDBdétaché. FRAM entière14fb172f…2aa9 inchangée depuis R4-R ; STARTED epoch4/tx5/run5 conservé. Aucun LIVE opérationnel ni cumulatif A04-02/M014-H restauré automatiquement. Les états précédents ci-dessous sont historiques.'''+t[b:];t+='''

## R4-S — bilan final du 16/10/2026

- **OBSERVÉ PHYSIQUE BORNÉ / phase1 PASS** : code c2395d9,10,001045s,265669XYZ/33209TS ; work333/service1804/gap1902µs, seuils1000/2500/5000 conservés, FIFOmax58/erreurs0. Cinq fenêtres diagnostiques,808RTU RAM cohérents, oracle4096 exact ; autoritéfalse, aucun B3 valide autoritatif. Suivi compteur/DWT :99contrôles,résidumax10µs/seuil100. Deux essais antérieurs échoués conservés séparément (dérive109µs, puis service2511µs) ; causes corrigées et attribuées.
- **VALIDÉ LOGICIEL** :141tests/main,ARM,profil/O2/ASan/UBSan/oracles PASS. Feuilles de calcul-O2 seulement profilR4S ; V1/R2 inchangés. **BLOCKED phase2** : retrait R4-M/R4-P sur copie authentique fait génération journal26→0→4 aprèsRENEW ; runfloor5 préservé/epoch4→5. Anciennes gardes refusent14fb. Décision structurante requise sur « aucun reset des compteurs », aucune exception supposée. Étude locale86d0f4b isolée/nonpoussée ; image04 jamais flashée. Phase3 nonexécutée ; R4global/R6 ouverts.
- **RESTAURÉ / PRÉSERVÉ** : M016 et FRAM intégrales exactes aux SHA ci-dessus ;0écritureFRAM/0admissionR2, historique/tail exacts, aucuneSD/RS485/HP. Reprise après décision publiée et preuve d'applicabilité sur copie ; ne pas rejouer la phase1 inchangée. Rapport/preuves R4S publiés pour contrôle indépendant ChatGPT ; aucune qualification de jalon par Codex.
''';f.write_text(t)
f=g/'ROADMAP_TR2_VERS_SUPERVISION_VIBRATOIRE.md';t=f.read_text();lines=t.splitlines();
for i,l in enumerate(lines):
 if l.startswith('| **R3 —'):lines[i]='| **R3 — FIFO/calcul/B3** | **LOGICIEL VALIDÉ ; R4-S DIAGNOSTIQUE PHYSIQUE PASS** | Provider suivi compteur/DWT, fenêtres diagnostiques réelles ; autorité R2 absente. |141tests/ARM/sanitizers ;10s/265669XYZ, oracle4096 exact,5fenêtres/808RTU RAM ; aucun B3 LIVE valide autoritatif. [R4-S](../05_Firmware/LIVE_R4S.md). |'
 if l.startswith('| **R4 —'):lines[i]='| **R4 — Temps réel** | **R4-S PHASE1 PASS ; PHASE2 BLOCKED** | Work333/service1804/gap1902µs sous bornes1000/2500/5000 ; charge readonly seulement. | Retrait journal sur copie : génération26→0→4 ; décision sur interdiction du reset requise. STARTED epoch4/tx5/run5 et FRAM14fb inchangés, M016 restauré exact. Phase3 nonexécutée ; R4global/R6 ouverts. [R4-S](../05_Firmware/LIVE_R4S.md). |'
f.write_text('\n'.join(lines)+'\n')
for name in ['publish.py','readback.py']:
 f=p/name;t=f.read_text().replace('MSM_vibe-R4S-service-budget-safe-recovery','MSM_vibe-R4S-publication')
 if name=='publish.py':
  t=t.replace("{'.json','.log','.py'}","{'.json','.log','.py','.c'}")
  t=t.replace("if not d.is_dir():continue","if not d.is_dir() or d.name not in {'probe-build-01','probe-build-02','probe-build-03'}:continue")
  t=t.replace('SHA256SUMS couvre tous les fichiers sauf lui-même.','''Les logs maintenance/recovery/probe-build-04..06 et model-hashes relèvent d'une
étude hôte NON AUTORISÉE SUR CIBLE ; code local86d0f4b, image04 jamais flashée
ni exportée ici. Ils ne prouvent aucune écriture physique ni conformité phase2.
Le contre-exemple indépendant counterexample_model.c utilise main inchangé ;
son application générique sur RAM ne vaut pas autorisation de capture.
Restoration-summary.json décrit l'état final matériel, aucune phase2/3 physique.
SHA256SUMS couvre tous les fichiers sauf lui-même.''')
 f.write_text(t)
print('FINAL DOCS WRITTEN',now)
