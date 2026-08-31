# FT-CMD-02 — Source normative consolidée

## 1. Références normatives

Source principale : `Modbus RTU/01_Specification_source/bloc5.md` V1.

Règles pertinentes :
- §4.2 Idempotence : une commande avec un `transaction_id` déjà traité ne doit jamais être exécutée une seconde fois ; le capteur doit répondre avec un statut terminé en réutilisant le résultat précédent ;
- §7.2 : la centrale doit modifier `cmd_request_transaction_id` à chaque nouvelle commande ; ce champ sert notamment à distinguer deux commandes successives identiques, éviter l'ambiguïté en cas de répétition de trame et permettre la corrélation stricte requête/réponse ;
- §7.8 : `cmd_active_transaction_id` expose l'identifiant de la commande active / dernière prise en compte ;
- §9.2 : une réponse n'est considérée valide par la centrale que si `cmd_active_transaction_id == transaction_id envoyé` ;
- §10.1 : une commande est considérée comme nouvelle si les conditions de soumission, de code, d'identifiant valide et de contexte compatible sont réunies.

## 2. Exigences consolidées

### CMD02-IDEM-001 — Rejeu d'un identifiant déjà traité

**Classification : `COVERED` pour la réutilisation du résultat ; `CONDITIONAL` pour la preuve de non-réexécution.**

Un `transaction_id` déjà traité ne doit pas donner lieu à une seconde exécution. Lors de son rejeu, le résultat précédent doit être réutilisé et le traitement exposé comme terminé.

La réutilisation du résultat est directement observable dans le Bloc 5. La preuve stricte de l'absence de seconde exécution nécessite soit une instrumentation, soit une commande dont l'effet permette de distinguer une exécution d'un simple rejeu.

### CMD02-IDEM-002 — Nouvel identifiant pour nouvelle commande

**Classification : `COVERED`.**

Deux commandes successives nouvelles doivent utiliser des `transaction_id` différents. Avec un nouveau `transaction_id` et les autres conditions de soumission satisfaites, le Bloc 5 doit traiter la requête comme une nouvelle transaction et exposer ce nouvel identifiant.

### CMD02-CORR-001 — Corrélation nominale

**Classification : `COVERED`.**

La réponse associée à une requête n'est recevable côté centrale que si `cmd_active_transaction_id` correspond exactement au `transaction_id` envoyé.

La validation capteur vérifie donc que l'identifiant exposé dans `cmd_active_transaction_id` correspond à la transaction effectivement prise en compte.

### CMD02-MEM-001 — Profondeur/durée de mémoire d'idempotence

**Classification : `NOT_DEFINED`.**

La V1 ne définit ni nombre minimal d'identifiants à mémoriser, ni durée de rétention, ni fenêtre temporelle, ni règle de vieillissement. Aucun test ne doit imposer une profondeur arbitraire.

Le cas testable retenu est le rejeu immédiat d'une transaction déjà terminée.

### CMD02-PAYLOAD-001 — Réutilisation du même identifiant avec requête différente

**Classification : `NOT_DEFINED`.**

La V1 impose l'idempotence pour un `transaction_id` déjà traité, mais ne décrit pas explicitement le cas incohérent où le même identifiant est rejoué avec un `cmd_request_code` ou des paramètres différents. Aucun ordre de priorité entre « rejeu de transaction » et « requête incohérente » n'est ajouté par la validation.

Ce point est conservé comme dette normative pour une future révision de la spécification.

## 3. Délégations et exclusions

- état final exact et historique : FT-CMD-03 ;
- concurrence avec une commande active : FT-CMD-04 ;
- effets spécifiques des commandes : FT-CMD-05 à FT-CMD-07 et FT-INT ;
- persistance de la mémoire d'idempotence après reboot : FT-PER ;
- validité exacte du domaine `transaction_id` : `NOT_DEFINED` déjà tracé par FT-CMD-01.

## 4. Principe d'essai

Pour éviter d'inventer une durée de mémorisation, les essais de rejeu sont effectués immédiatement après terminaison de la transaction initiale. La preuve de non-réexécution ne doit être déclarée conforme que si l'environnement fournit une observation discriminante suffisante.
