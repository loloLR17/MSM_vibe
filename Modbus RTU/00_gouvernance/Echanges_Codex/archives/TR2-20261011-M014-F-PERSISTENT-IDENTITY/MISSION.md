---
mission_id: TR2-20261011-M014-F-PERSISTENT-IDENTITY
status: READY
base_ref: main
base_sha: 24bb47bf17c8e36d1799227900de61d517e92597
created_at_utc: 2026-10-11T21:35:00Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M014-F — Identité persistante V1, migration transactionnelle et composition production

## Référence et autonomie
Appliquer AGENTS.md, protocole V2-B3, roadmap et état courant de main ; vérifier HEAD distant/local, état Git, stashes, rapport M014-E BLOCKED, PROVISIONING_PRODUCTION_M014_E.md, gels V1 et code réel avant toute action. Autorisation permanente de développement/commit/push/flash/debug confirmée et explicite. Travailler en autonomie sans validations répétitives, poursuivre les lots indépendants en cas de blocage. Un seul pilote sur main. Archiver M014-E mission/rapport sans écraser d'archive existante si nécessaire selon protocole.

## Décision d'architecture approuvée
Retenir une **évolution versionnée du layout transactionnel logique existant**, et non un second domaine FRAM physique indépendant. Conserver un seul mécanisme de publication/recovery et l'atomicité identité + configuration. Les 51 818 octets du layout historique sont intégralement occupés : ne réutiliser aucun octet, ne pas écraser BootIntent ni CampaignData. Proposer et justifier une extension logique avec offsets, tailles, géométrie et version de format explicites, dans les limites physiques vérifiées ; prévoir lecture ancien format et migration explicite contrôlée, sans formatage/écriture au boot. Ne pas altérer les gels Modbus V1.

Identité matérielle STM32 UID96 intégral, immuable, liée à l'identité fonctionnelle ; DeviceId uint32 unique attribué explicitement par registre d'attribution projet (pas hash/troncature UID, pas constante) ; identité B0 complète réelle et versionnée ; adresse esclave 1..247 indépendante et persistante ; distinguer version layout interne/version protocole V1. Définir codec portable sans padding/endianness native, magic/version/longueur/génération/intégrité et contrôles d'identité. Ne pas inventer un DeviceId de production ni déclarer une unicité de parc sans registre autoritatif.

## Travail attendu
1. Audit exhaustif des allocations physiques/logiques FRAM, schéma transactionnel (2 slots + superblocks), métadonnées, anciennes images et chemins CampaignData/BootIntent. Définir et publier spécification technique du layout V2 interne, migration V1→V2, ownership exclusif et plan de sauvegarde/restauration. Démontrer calcul des bornes et compatibilité avant codage.
2. Implémenter codec/inspecteur autoritatif et moteur de provisioning initial EMPTY et migration, explicitement déclenchés hors boot. Réutiliser les transactions existantes ; protéger les données anciennes et assurer rollback/recovery lors des coupures, mismatch UID, versions incompatibles, génération périmée, double provisioning, corruption et idempotence. Ne jamais transformer EMPTY en VALID par défaut.
3. Composer les services STM32 de production réels progressivement : FRAM/config/identité/transport/Modbus B0/B1 et bulkSD si média/autorités disponibles. Ne pas contourner weak binding NOT_AVAILABLE par des fixtures, faux READY, faux DeviceId ou UID tronqué. Diagnostiquer précisément les dépendances non disponibles et poursuivre le code/test indépendant. Ne pas assimiler B0/B1 à qualification de tous blocs opérationnels.
4. Tests unitaires/fault-injection hôte, cross-build ARM, tests .NET proportionnés ; vérifier non-régression et comportement sans média. Préparer et si les préconditions réelles sont remplies, exécuter en autonomie flash/debug qualifiés : identifier STM32, image exacte/hash, sauvegardes/restauration du diagnostic E, voie de programmation validée ; aucun HOTPLUG CubeProgrammer (risque NVIC connu). Préserver le firmware diagnostique si l'image production n'est pas restaurable.
5. Si la cible de production est réellement opérationnelle et les données/identités/provisioning sont autoritatifs, préparer puis exécuter des essais HP via SSH bornés sur COM7 avec maître exclusif, logs et hashes nouveaux, sans rejouer M014-C/M013-I, sans GDB pendant trafic. Ne pas ouvrir COM7 tant que le firmware ne fournit pas réellement les blocs requis ; ne pas inventer de succès physique. Aucun effacement massif, Option Bytes, déprotection, écriture destructive inhabituelle ou changement de câblage. Interventions physiques nécessitant l'utilisateur seulement si nécessaires.

## Escalades réelles
Arrêter seulement pour incompatibilité normative, choix structurant non couvert par l'architecture approuvée, allocation impossible démontrée, destruction de données préexistantes, risque matériel non maîtrisé ou geste physique indispensable. Les validations logicielles courantes et flash/debug standard sous garde-fous ne demandent aucune nouvelle approbation. Si une valeur de production ou registre d'attribution manque, développer les mécanismes sans inventer l'identité et signaler précisément ce seul blocage ; ne pas immobiliser le reste.

## Critères et rapport
Publier les spécifications de format, allocation/migration et code/test validés ; consigner commandes réelles, résultats, preuves, SHAs, statut logiciel et physique distincts. Mettre à jour ETAT_COURANT_TR2.md et DERNIER_RAPPORT.md en respectant la propriété Codex ; relecture distante des livrables. M014 demeure IN PROGRESS tant que lectures périodiques et perte/reprise physiques ne sont pas démontrées. En cas de blocage/quota, livrer un point de reprise exploitable et préserver tous travaux.
