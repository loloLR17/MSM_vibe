---
mission_id: TR2-20261014-P12H3H-SDMMC1-COLDSTART10CM
status: READY
base_ref: main
base_sha: d4f451c1ced5c39f2df9abf690a71be4bb8ce53e
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# P12/H3h — Contrôle ciblé après coupure électrique complète, fils Dupont 10 cm

## Fait nouveau et objectif
L'utilisateur confirme avoir **coupé complètement l'alimentation de la seconde Nucleo en débranchant ST-LINK et USB-C natif, puis reconnecté les deux câbles**, avec la microSD de test 32 Go laissée insérée et le câblage SDMMC1 à fils Dupont 10 cm inchangé. Ce cycle électrique est **postérieur** au rapport FAILED de la mission `TR2-20261014-P12H3H-SDMMC1-CABLE10CM-COMPARISON`, dont le seul reset était logiciel et qui n'avait réalisé aucun transfert (No Media persistant).

Objectif : déterminer **sans autre manipulation physique** si un vrai redémarrage électrique restaure l'accès au média et, si oui, exécuter le comparatif court 10 cm avec le PoC 20 cm. Ne pas répéter les tests déjà acquis ni confondre disponibilité et qualification.

## Préparation et invariants
Lire AGENTS.md, GOUVERNANCE_CHATGPT_CODEX.md, rapport précédent et preuves, état courant ; vérifier HEAD, Git, identités. Cible autorisée exclusivement seconde NUCLEO-U575ZI-Q ST-LINK `0032001F3235510C37333439`, UID `001B0046/38425018/20333157`. Média autorisé exclusivement SanDisk test 32 Go CID `03534453/44333247/858BF092/3C01927A`, capacité `62333952` secteurs 512 octets. **Ne jamais accéder** à la Nucleo principale ST-LINK `003500463235510B37333439`, sa FRAM ou microSD 512 Go. Aucun formatage, mass erase, Option Bytes/RDP, écriture arbitraire ni changement de câble/SD/soudure.

Réutiliser firmware tiers original Release VILLO88/NUCLEOU575 commit `1e3c9183bad023ebf7d234134823d9270a0e8b64`, BIN SHA-256 `2a2fe744ab926789ae180b74f2a250b2a9202329b5afde51a1155cdd69d54c1a`, déjà flashé ; vérifier identité/image sans reflasher inutilement. Conserver bus 4 bits/12 MHz calculés, USB natif, paramètres et outils de comparaison inchangés. SB19/SB22 déjà pontés selon vérification utilisateur/Codex ; ne pas demander de recontrôle.

## Séquence courte
1. **Avant tout GDB/reset**, relever sous Windows l'énumération USB et la disponibilité effective du disque (Size > 0 vs No Media) à la suite de la coupure électrique complète ; préserver l'état initial comme preuve. Vérifier la cible, l'image et le CID avant toute écriture. Un CID seulement mémorisé dans un handle ne prouve pas la disponibilité réelle.
2. Si No Media persiste après le vrai power-cycle : capturer uniquement le diagnostic non destructif minimal utile (état carte/SDMMC, erreurs, énumération) ; **aucun nouveau reset automatique, aucune écriture**, arrêt et rapport BLOCKED/FAILED selon faits. Ne pas conclure à l'inefficacité des fils 10 cm.
3. Si le média est accessible : reprendre **le même test court** que la comparaison précédente : 16 × 8192 octets = 128 KiB d'écritures alignées dans la plage test explicitement identifiée et protégée, flush, fermeture/réouverture, relecture et comparaison intégrales, SHA, temps écriture/lecture, CRC, No Media, état carte. Au plus trois cycles consécutifs, sans reset entre cycles ; **arrêter au premier CRC, échec d'intégrité ou No Media** et préserver les preuves. Ne pas lancer endurance 600 s/7 jours, M017, tests GPIO ou suite complète. Pas de modifications firmware/HAL/bus/USB ni correction de recovery dans cette mission.
4. Comparer explicitement au PoC 20 cm : 128 KiB write+flush 3178,7446 ms (~41 234 B/s) et read 187,4372 ms (~699 285 B/s), succès intègres ponctuels mais CRC/No Media. Distinguer performances applicatives hôte/USB et débit SD. Si le comparatif n'a pas pu démarrer, ne produire aucun gain chiffré.

## Décision et livraison
Statut DONE uniquement pour une **amélioration bornée démontrée** (cycles complets sans erreur, sans reset intermédiaire, intégrité exacte et disponibilité conservée), **sans qualification de fiabilité longue durée ni industrielle**. Si échec/erreur ou données insuffisantes, FAILED ou INCONCLUSIVE selon preuves. Ne pas masquer les erreurs ou faire plusieurs resets jusqu'à réussite.

Autorisations `target_branch: main`, `allow_commit: true`, `allow_push: true`, `allow_flash: true`, `allow_debug: true` avec garde-fous AGENTS.md ; flash permis mais normalement inutile. Utilisateur seul responsable des gestes physiques. Préserver dépôts/worktrees/stashes et baseline. Publier dans `DERNIER_RAPPORT.md` les observations horodatées, preuves minimales, comparaison et état laissé, puis commit/push et relecture distante ; contrôle indépendant ChatGPT ultérieur. Ne pas démarrer automatiquement une autre mission.
