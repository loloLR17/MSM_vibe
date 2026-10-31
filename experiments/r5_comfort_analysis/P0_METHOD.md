# Fiche P0 — état réel au31octobre2026

**P0 NON SATISFAITE.** La méthode complète ne peut pas encore être reconstruite
et vérifiée sur les sources autorisées actuellement disponibles. La demande de
source complémentaire a reçu la réponse utilisateur «Aucun accès disponible
actuellement». Ce blocage concerne le nom et la comparaison normatifs ; les
calculs PC préliminaires non pondérés sont poursuivis et livrés séparément.

## Références examinées

- [ISO20283-5:2016, OBP officiel](https://www.iso.org/obp/ui?_escaped_fragment_=iso:std:iso:20283:-5:ed-1:v1:en) :
  aperçu jusque définitions §3 ; renvois ISO2631-2 et ISO8041 non datés, avec
  règle d'emploi de la dernière édition pour ces renvois (§2). Ne pas importer
  aveuglément la2631-2:2003 alors que l'édition2026 existe. Les §7/8.1 et
  l'annexeA ne sont pas exposés dans cette consultation.
- [Aperçu officiel du membre national SIST, ISO20283-5:2016](https://preview.sist.si/sist-preview/68125/1e17a626b1f04c9d9674b2f1943ddfc6/ISO-20283-5-2016.pdf) :
 10pagesPDF disponibles, arrêt avant les clauses nécessaires de méthode et
 équivalence. [Aperçu iTeh](https://cdn.standards.iteh.ai/samples/iso/iso-20283-5-2016/715aab72f0b842e183b05e2012608f8f/iso-20283-5-2016.pdf)
 limité à5pages. Pas de lecture complète prétendue.
- [ISO2631-2:2026, OBP officiel](https://www.iso.org/obp/ui?_escaped_fragment_=iso:std:iso:2631:-2:ed-3:v1:en) :
  nouvelle édition qui remplace2003 ; annexeA Wm référencée mais non accessible.
  Le descriptif d'évolution ne constitue pas une preuve des coefficients inchangés.
- [Aperçu autorisé ISO2631-2:2003](https://cdn.standards.iteh.ai/samples/23012/96ddf9d86d084af4af8f733ed04178c4/ISO-2631-2-2003.pdf) :
 9pagesPDF, renvoie à l'annexeA mais s'arrête au corps principal. Pas de source
 normative complète des coefficients ou d'un oracle Wm utilisable.
- [ISO8041-1:2017, fiche officielle](https://www.iso.org/standard/70648.html) :
  référence d'instrumentation, tolérances et validation ; fiche insuffisante pour
  qualifier le filtre et l'instrument. Pas d'instrument conforme déclaré.
- [BV NR467 partieF, juillet2026](https://rulesexplorer-docs.bureauveritas.com/documents/nr467/jul2026/467-NR_PartF_2026-07.pdf) :
  texte officiel accessible. Le chapitre6§2.1.3 lie la voie de vitesse RMS pondérée
 1–80Hz à ISO20283-5 ; cela ne définit pas tous les filtres/équivalences nécessaires.
  Le repérage textuel du PCmachine occupé en permanence se trouve en **section3,
 tableau4** (pagePDF247/pageimprimée245), et nonsection2 citée dans la mission.
  Ce constat n'entraîne pas une modification du contrat ni l'affichage de ses
 repères. L'occupation réelle du local et sa correspondance restent à documenter.

Les recherches web ont aussi révélé des copies non officielles et une étude
universitaire ; elles ne remplacent pas la vérification des clauses requises.
Aucun miroir non autorisé n'a servi de source à une implémentation normative.
Les PDF complets et captures de textes sous droit d'auteur ne sont pas republiés.

## Éléments indispensables à la levée de P0

1. Accès licite aux clausesISO20283-5:2016 §7 et§8.1/équation d'équivalence,
   annexeA et renvois : grandeur exacte, unités, sens du coefficient, durées,
   directions et sélection pour l'évaluation. La vitesse **physique** obtenue
   par intégration ne doit pas être confondue avec un équivalent de présentation.
2. AnnexeA ISO2631-2:2026 (ou autre édition **explicitement justifiée** par la
   référence applicable) : fonctions exactes, paramètres/coefficient et tables ;
   ISO8041 pertinente pour tolérances et points de référence indépendants.
3. Mise en oeuvre versionnée, tests aux points de référence, contrôle indépendant
   des équivalences et preuve de méthode avant toute comparaison à un repèreBV.
4. Pour une comparaison de local : confirmer classe/occupation et directions,
   puis afficher strictement la portée indicative, avec amplitude/pose/horloge
   non étalonnées. Pas de pourcentage de minutes assimilé à celui des locaux.

Aucune norme n'est achetée et aucune nouvelle manipulation physique n'est demandée
pour ce traitement. Le blocage doit être rendu dans DERNIER_RAPPORT.md après la
publication des avancées non normatives réellement calculées.
