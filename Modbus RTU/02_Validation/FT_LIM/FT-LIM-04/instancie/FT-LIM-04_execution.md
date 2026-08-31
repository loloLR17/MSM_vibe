# FT-LIM-04 — Procédure d'exécution

## Préconditions générales
- utiliser une plateforme de test où les commandes sont autorisées ;
- acquérir un état initial du Bloc 5 ;
- utiliser un `transaction_id` neuf pour chaque nouvelle commande sauf essai d'idempotence ;
- ramener `submit` à 0 avant de préparer un nouveau front lorsque nécessaire ;
- vérifier les préconditions spécifiques avant soumission ;
- pour les commandes potentiellement destructives (10/11), employer un environnement explicitement autorisé.

## Séquence standard
1. Lire le Bloc 5 et capturer état/résultat/historique.
2. Préparer code, transaction ID et paramètres.
3. Écrire `confirm_key` si requis.
4. Vérifier `submit=0` avant nouvelle soumission lorsque le firmware ne l'a pas déjà remis à zéro.
5. Créer le front `submit 0→1`.
6. Observer `cmd_active_code`, `cmd_active_transaction_id`, `cmd_status`, `cmd_result_code`, historique final et effets fonctionnels attendus.
7. Ne jamais exiger que `submit` soit encore à 1 après prise en compte.
8. Vérifier absence d'effet parasite sur les autres commandes/états.

## Oracles
- succès : `cmd_result_code=0` lorsque toutes les préconditions sont remplies et que l'action elle-même réussit ;
- refus : utiliser uniquement le code explicitement défini pour la cause testée ;
- si plusieurs causes de refus sont présentes simultanément, le cas est invalide pour attribution d'un code précis : isoler une seule cause ;
- un code réservé/commande inconnue ne doit pas devenir une exception Modbus inventée ;
- pour idempotence, la preuve essentielle est l'absence de seconde exécution et la réutilisation du résultat précédent.

## Cas conditionnels
Marquer `N/A — ENVIRONMENT_NOT_CHARACTERIZED` ou un motif plus précis quand la cause ne peut pas être produite de manière fiable : horloge indisponible, mémoire insuffisante, défaut critique, défaut acquittable/non acquittable, commande annulable, opération critique en cours, extension masque autotest.

## Sécurité des resets
Les essais des commandes 10 et 11 ne sont exécutés que sur plateforme où reset logiciel et RAZ statistiques sont acceptables. Sinon, conserver les cas documentés et marquer N/A avec justification ; ne jamais simuler un PASS.
