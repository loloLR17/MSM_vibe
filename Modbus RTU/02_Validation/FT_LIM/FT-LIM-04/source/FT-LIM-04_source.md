# FT-LIM-04 — Source normative

## Exigences dérivées de la V1

### LIM04-RQ-001 — Domaine du code commande
`cmd_request_code` : 0 = aucune commande ; 1..11 = commandes définies ; 12..65535 = réservés. Une valeur réservée sur ce registre RW n'est pas une erreur d'accès Modbus ; lorsqu'elle est soumise, elle doit être traitée fonctionnellement comme commande inconnue/refusée selon la V1.

### LIM04-RQ-002 — Transaction ID
Chaque nouvelle commande doit utiliser un `transaction_id` valide et différent. Un `transaction_id` déjà traité ne doit jamais provoquer une seconde exécution ; le résultat précédent est réutilisé.

### LIM04-RQ-003 — Front de soumission
Une commande n'est évaluée que sur front montant `submit: 0→1`. Le maintien à 1 ne provoque aucune répétition. Le firmware remet automatiquement `submit` à 0 après prise en compte.

### LIM04-RQ-004 — Conditions minimales de prise en compte
La commande n'est considérée que si `submit=1`, `cmd_request_code!=0`, `transaction_id` valide et contexte compatible.

### LIM04-RQ-005 — Paramètres inutilisés
`param1`, `param2`, `param3` valent 0 lorsqu'ils ne sont pas utilisés par la commande. Aucune valeur métier non définie n'est inventée.

### LIM04-RQ-006 — Confirmation
`confirm_key=0x0000` signifie absence de confirmation et `0xA55A` confirmation valide. Les commandes protégées V1 sont 10 et 11. Sans confirmation valide, résultat fonctionnel 9.

### LIM04-RQ-007 — Commande 1
Appliquer configuration préparée : paramètres nuls ; acquisition arrêtée ; configuration préparée complète, valide et CRC conforme. Les détails CRC/état sont couverts par FT-LIM-03.

### LIM04-RQ-008 — Commande 2
Synchroniser heure : aucun paramètre Bloc 5 ; heure préparée présente ; horloge disponible. Résultat 19 si synchronisation préparée absente ; 12 si horloge indisponible.

### LIM04-RQ-009 — Commande 3
Démarrer acquisition : configuration active valide, SD exploitable, mémoire suffisante, absence de défaut critique bloquant, acquisition non déjà active. Les refus V1 comprennent 22, 6, 7, 8 et 3 selon la cause.

### LIM04-RQ-010 — Commande 4
Arrêter acquisition : acquisition active. Si aucune acquisition active, résultat 21.

### LIM04-RQ-011 — Commande 5
Autotest : `param1=0` désigne l'autotest standard complet. Une autre valeur ne constitue un masque de sous-tests que si cette extension est implémentée ; aucune sémantique de masque n'est imposée en V1. Les résultats possibles explicitement cités incluent 3, 10 et 11.

### LIM04-RQ-012 — Commande 6
Acquittement : `param2=0` pour acquittement unitaire ; `param2=1` pour acquittement global des défauts acquittables. Le défaut ciblé doit être présent et acquittable. Un défaut non acquittable conduit au résultat 16.

### LIM04-RQ-013 — Commandes 7 à 9
Rafraîchir indicateurs/états, entrer en maintenance et sortir maintenance sont testées uniquement avec les préconditions et effets explicitement définis dans la V1. Aucun état implicite n'est inventé.

### LIM04-RQ-014 — Commandes protégées 10 et 11
Reset logiciel contrôlé et RAZ statistiques exigent `confirm_key=0xA55A`. La commande 10 exige en plus acquisition arrêtée et absence d'opération critique non terminée. Les autres préconditions ne sont pas extrapolées.

### LIM04-RQ-015 — Annulation
`cancel_request` demande l'annulation seulement si la commande courante est annulable. Le bit 9 de `cmd_engine_flags` expose le support d'annulation. Si non annulable, résultat 15. Aucune commande particulière n'est déclarée annulable sans preuve normative/observable.

### LIM04-RQ-016 — Nettoyage
`clear_request_fields` demande la remise à zéro des champs de requête. Cette action ne doit pas être confondue avec l'exécution d'une nouvelle commande.

### LIM04-RQ-017 — Résultats
Les codes résultat 0..22 sont ceux de la table V1 ; 23..65535 sont réservés. FT-LIM-04 n'invente aucun code ni détail de résultat.

## Non défini / à ne pas inventer
- liste exhaustive des commandes annulables ;
- sémantique des masques d'autotest autres que `param1=0`, sauf extension effectivement implémentée et documentée ;
- temporisations d'exécution ;
- durée minimale d'exposition des états intermédiaires ;
- code `cmd_result_detail` pour chaque cause ;
- préconditions non écrites pour les commandes 7 à 9 et 11.
