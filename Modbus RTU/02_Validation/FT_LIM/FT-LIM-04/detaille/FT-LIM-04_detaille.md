# FT-LIM-04 — Détail des cas génériques

## LIM04-G01 — Domaine `cmd_request_code`
Vérifier 0, chaque code 1..11 et des valeurs réservées représentatives. Les valeurs réservées sont écrites sur un RW valide puis soumises ; l'oracle est fonctionnel, jamais une exception Modbus inventée.

## LIM04-G02 — Transaction ID valide et renouvelé
Soumettre deux commandes successives avec deux IDs distincts et vérifier la corrélation requête/résultat.

## LIM04-G03 — Idempotence
Représenter exactement la même commande avec un `transaction_id` déjà traité. Vérifier absence de seconde exécution et restitution du résultat précédent.

## LIM04-G04 — Front montant `submit`
Préparer la requête, créer un front 0→1, vérifier une seule prise en compte. Ne pas exiger que le bit reste à 1 après lecture.

## LIM04-G05 — Maintien de `submit`
Lorsque la séquence d'observation le permet, vérifier qu'un niveau 1 maintenu ne réexécute pas la commande. Si le firmware remet immédiatement le bit à 0, l'idempotence G03 fournit la preuve principale de non-répétition.

## LIM04-G06 — Confirmation protégée
Pour codes 10 et 11 : absence de confirmation / clé incorrecte / `0xA55A`. Une clé valide n'implique succès que si les autres préconditions sont satisfaites.

## LIM04-G07 — Commande 1
Vérifier au moins : baseline valide ; acquisition en cours ; configuration préparée incomplète. Les défauts CRC/validation détaillés restent FT-LIM-03.

## LIM04-G08 — Commande 2
Vérifier heure préparée disponible et absence d'heure préparée. L'indisponibilité d'horloge n'est testée que si elle peut être provoquée de manière contrôlée.

## LIM04-G09 — Commande 3
Vérifier baseline de démarrage avec configuration active valide. Tester séparément, lorsque contrôlables : aucune configuration active valide, SD absente, mémoire insuffisante, défaut critique, état incompatible.

## LIM04-G10 — Commande 4
Vérifier arrêt avec acquisition active et refus lorsque l'acquisition n'est pas active.

## LIM04-G11 — Commande 5
Tester `param1=0`. Les valeurs non nulles sont conditionnelles à l'extension de masque de sous-tests ; sinon N/A, sans comportement inventé.

## LIM04-G12 — Commande 6
Tester les modes explicitement définis `param2=0` et `param2=1`, avec défaut acquittable lorsque l'environnement le permet. Tester le résultat 16 uniquement avec un défaut non acquittable réellement caractérisé.

## LIM04-G13 — Commandes 7, 8 et 9
Tester l'acceptation et les effets explicitement observables dans un contexte valide. Les contextes négatifs ne sont ajoutés que s'ils sont normativement définis.

## LIM04-G14 — Commandes 10 et 11
En plus de la confirmation G06, tester les préconditions propres explicitement définies. Pour le reset logiciel, acquisition arrêtée et absence d'opération critique non terminée sont requises.

## LIM04-G15 — Annulation
Si `cmd_engine_flags.bit9=1`, exercer `cancel_request` sur une commande réellement en cours et vérifier le comportement documenté. Si bit9=0, la demande d'annulation doit produire le résultat 15 lorsque le moteur la traite. Les cas non provoquables sont N/A.

## LIM04-G16 — `clear_request_fields`
Demander le nettoyage des champs de requête et vérifier leur remise à zéro sans exécution parasite.

## Règles transverses
- transaction ID neuf sauf test d'idempotence ;
- contrôler les préconditions avant chaque essai ;
- une valeur fonctionnelle invalide sur RW ne devient pas une erreur d'accès Modbus ;
- ne jamais conclure sur un effet destructif sans moyen de test sûr ;
- les commandes reset sont exécutées uniquement sur plateforme d'essai autorisant ce comportement.
