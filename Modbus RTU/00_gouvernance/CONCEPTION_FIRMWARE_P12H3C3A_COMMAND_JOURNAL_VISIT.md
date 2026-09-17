# P12-H3c3-A — Évolution minimale du contrat CommandJournal

## 1. Objet

Préparer le remplacement du store dense par le store borné H3c2 sans introduire encore l'éviction ni modifier le format persistant.

## 2. Problème

`command_boot_recovery_scan()` dépend actuellement de `CommandJournalStore.max_transaction_id` et parcourt le domaine numérique des IDs. Cette dépendance appartient à l'ancien layout dense.

Le recovery applicatif doit pouvoir parcourir les transactions connues sans connaître :
- le domaine 1..65535 ;
- le nombre de slots physiques ;
- leur layout.

## 3. Décision de contrat

Ajouter à `CommandJournal` une primitive d'énumération logique :

```c
Tr2Result (*visit)(void *context,
                   CommandJournalVisitor visitor,
                   void *visitor_context);
```

avec un callback recevant chaque `CommandJournalEntry` courante exactement une fois.

Règles :
- l'ordre d'énumération n'est pas contractuel ;
- seules les entrées logiquement présentes sont visitées ;
- une erreur de lecture/corruption est propagée ;
- le visitor peut interrompre l'énumération en retournant une erreur ;
- aucun détail de slot physique n'est exposé.

## 4. Migration

Dans H3c3-A :
1. ajouter le contrat `visit`;
2. l'implémenter sur le store dense actuel ;
3. migrer `command_boot_recovery_scan()` vers `visit`;
4. conserver `find/reserve/set_recovery_context/mark_started/complete/latest_completed` inchangés ;
5. conserver le record v2 de 66 octets inchangé ;
6. conserver le layout dense inchangé.

Ainsi la tranche est purement architecturale et doit être neutre fonctionnellement.

## 5. Critères de validation

- tous les tests P7/P8 existants restent verts ;
- test dédié : enumeration vide ;
- test dédié : plusieurs transactions connues visitées exactement une fois ;
- propagation d'une erreur du visitor ;
- boot recovery ne dépend plus de `max_transaction_id`.

## 6. Hors scope

- fenêtre 256 ;
- admission_order ;
- éviction ;
- nouveau format record ;
- média STM32 ;
- migration de données.

Prochaine tranche après validation : H3c3-B — format persistant borné et admission_order.
